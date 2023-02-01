/*
https://pandia.ru/text/77/275/21057-8.php
*/
#include <iostream>
#include <errno.h>
#include <locale>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <io.h>
#include <fcntl.h>
using namespace std;

struct sym
{
	//unsigned char ch;
	//char ch;
	wchar_t ch;
	float freq;
	char code[255];
	sym* left;
	sym* right;
};

void Menu();
void Statistics(wstring String);
sym* MakeTree(sym* psym[], int couter_unique_simbols);
void MakeCodes(sym* root);
void CodeHuffman(wstring String, char* BinaryCode, sym* root);
void DecodeHuffman(char* BinaryCode, wchar_t* ReducedString, sym* root);
int chh;								//переменная для подсчета информация из строки
int couter_unique_simbols = 0;			//счётчик количества различных букв, уникальных символов
int couter_of_all_simbols = 0;			//счетчик количества всех знаков в файле
int arr_unique_simbols[256] = { 0 };	//инициализируем массив количества уникальных символов
sym simbols[256] = { 0 };				//инициализируем массив записей
sym* psym[256];							//инициализируем массив указателей на записи
float summ_of_all_freq = 0;				//сумма частот встречаемости
float Size_Encode = 0;					//сумма в битах сжатой строки
float сompression_ratio = 0;			//коэффицент сжатия строки
int _stateMenu;
//чтение из файла
wstring readFile(const char* filename)
{
	wifstream wif(filename);
	wif.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
	wstringstream wss;
	wss << wif.rdbuf();
	return wss.str();
}

int main()
{
	auto str = readFile("Input.txt");
	_setmode(_fileno(stdout), _O_U16TEXT);
	int str_size = str.size();
	char* String = new char[str_size];
	char* BinaryCode = new char[str_size*10];
	//char* ReducedString = new char[str_size+1];
	wchar_t* ReducedString = new wchar_t[str_size+1];
	String[0] = BinaryCode[0] = ReducedString[0] = 0;
	//schitivanie iz file
	FILE* stream;
	/*
	errno_t Input = fopen_s(&stream, "Input.txt", "r");
	char x;
	int i = 0;
	while ((feof(stream)==0))
	{
		fscanf_s(stream, "%c", &x);
		String[i] = x;
		i++;
	}
	String[i - 1] = '\0';
	fclose(stream);
	*/
	sym* symbols = new sym[couter_unique_simbols];				//создание динамического массива структур simbols
	sym** psum = new sym * [couter_unique_simbols];				//создание динамического массива указателей на simbols

	Menu();
	while (_stateMenu != 0)
	{
		Statistics(str);
		if (couter_unique_simbols > 2)
		{
			sym* root = MakeTree(psym, couter_unique_simbols);			//вызов функции создания дерева Хаффмана
			MakeCodes(root);						//вызов функции получения кода

			int count = 0;
			errno_t Output;// = fopen_s(&stream, "Output.txt", "w");

			switch (_stateMenu)
			{
			case 1:
				CodeHuffman(str, BinaryCode, root);	//кодирование исходной строки по дереву(создание бинарной строки)
				wcout << "Razmer ishodnogo file :\t" << couter_of_all_simbols * 8 << " bit\n";
				wcout << "Razmer Encode file : \t" << Size_Encode << " bit\n";
				сompression_ratio = ((couter_of_all_simbols * 8 - Size_Encode) / (couter_of_all_simbols * 8)) * 100;
				wcout << "Compression_ratio : \t" << сompression_ratio << "%\n";
				//errno_t Output = fopen_s(&stream, "Output.txt", "w");
				//уберем бинарный код из вывода
				//fprintf(stream, "Binary Code:\n%s\n", BinaryCode);
				count = 0;
				//fprintf(stream, "Encoding Code = ");
				Output = fopen_s(&stream, "Output.txt", "w");
				while (count < strlen(BinaryCode))
				{
					int bin_factor = 10000000;
					int temp = (BinaryCode[count++] - 48) * bin_factor;
					while (bin_factor > 1)
					{
						bin_factor /= 10;
						temp += (BinaryCode[count++] - 48) * bin_factor;
					}
					fprintf(stream, "%c", temp);
				}
				//fprintf(stream, "Compression ratio file = %f%%\n", сompression_ratio);
				fclose(stream);
				Menu();
				break;
			case 2:
				if (strlen(BinaryCode) == 0)
				{
					wcout << "\tCOMPRESSION FILE DOSENT EXIST\n";
					exit(-1);
				}
				else
				{
					DecodeHuffman(BinaryCode, ReducedString, root);
					//fprintf(stream, "\nDecoding string:\n%s\n", ReducedString);
					wofstream strm;                            // выходной поток-объект
					strm.open("DecodedFile.txt");    // открываем
					strm.imbue(std::locale(strm.getloc(), new std::codecvt_utf8<wchar_t>));	//устанавливаем кодировку
					wchar_t temp_ch;
					wstring test = ReducedString;
					for (int i = 0; i < test.size(); i++)
					{
						temp_ch = ReducedString[i];
						strm.put(temp_ch);
					}
					strm.close();
					Menu();
					break;
				}
			default:
				wcout << "\tWRONG CHOISE" << endl;
				Menu();
				break;
			}
		}
		//если строка из одного уникального символа
		else
		{
			int count = 0, temp;
			errno_t Output;
			wifstream win;
			wofstream wout;                           // выходной поток-объект
			switch (_stateMenu)
			{
			case 1:
				//т.к в входноой файл состоит из дного уник символа, то я просто каждые 8 байт заменяю 1 байтом
				//по сути можно даже весь файл сжать до одного байта
				Output = fopen_s(&stream, "Output.txt", "w");
				count = couter_of_all_simbols / 8;
				while (count > 0)
				{
					int temp = 11111111;
					count--;
					fprintf(stream, "%c", temp);
				}
				count = couter_of_all_simbols % 8;
				temp = 1;
				while (count > 1)
				{
					temp = temp * 10 + 1;
					count--;
				}
				fprintf(stream, "%c", temp);
				fclose(stream);
				сompression_ratio = (((float)couter_of_all_simbols * 8 - couter_of_all_simbols) / (couter_of_all_simbols * 8)) * 100;
				wcout << "Compression ratio file = " << сompression_ratio << "%" << endl;
				Menu();
				break;
			case 2:
				//Output = fopen_s(&stream, "DecodedFile.txt", "w");
				//тут я по сути никакого декодирования не делаю а просто переписываю входной файл в выходной
				win.open("Input.txt");
				wout.open("DecodedFile.txt");    // открываем
				wchar_t temp_ch;
				while (win.get(temp_ch))        // читать все символы, в том числе пробельные
					wout.put(temp_ch);
				win.close();
				wout.close();
				Menu();
				break;
			default:
				wcout << "\tWRONG CHOISE" << endl;
				Menu();
				break;
			}
		}
	}
	delete[] psum;
	delete[] String;
	delete[] BinaryCode;
	delete[] ReducedString;
	return 0;
}
//функция для выбора действия пользователя
void Menu()
{
	wcout << "Menu: " << endl
		<< "(0) exit" << endl
		<< "(1) compression" << endl
		<< "(2) decompression" << endl
		<< "enter: ";
	wcin >> _stateMenu;
}
//рeкурсивная функция создания дерева Хаффмана
sym* MakeTree(sym* psym[], int couter_unique_simbols)
{
	sym* temp;
	temp = new sym;
	temp->freq = psym[couter_unique_simbols - 1]->freq + psym[couter_unique_simbols - 2]->freq;
	temp->code[0] = 0;
	temp->left = psym[couter_unique_simbols - 1];
	temp->right = psym[couter_unique_simbols - 2];
	if (couter_unique_simbols == 2)
	{
		return temp;
	}
	else
	{
		//внесение в нужное место массива элемента дерева Хаффмана
		for (int i = 0; i < couter_unique_simbols; i++)
		{
			if (temp->freq > psym[i]->freq)
			{
				for (int j = couter_unique_simbols - 1; j > i; j--)
				{
					psym[j] = psym[j - 1];
				}
				psym[i] = temp;
				break;
			}
		}
	}
	return MakeTree(psym, couter_unique_simbols - 1);
}
//рекурсивная функция кодирования дерева
//которая имитирует кодирование символов инпут файла 
// по дереву, путем формирования символов 0 или 1(вместо битов)
// подсчитывая и возвращая их количество
//влево от узла устанавливаем 0, вправо 1
void MakeCodes(sym* root)
{
	if (root->left)
	{
		strcpy_s(root->left->code, root->code);
		strcat_s(root->left->code, "0");
		MakeCodes(root->left);
	}
	if (root->right)
	{
		strcpy_s(root->right->code, root->code);
		strcat_s(root->right->code, "1");
		MakeCodes(root->right);
	}
}
void Statistics(wstring String)
{
	couter_unique_simbols = 0;
	couter_of_all_simbols = 0;
	summ_of_all_freq = 0;
	//arr_unique_simbols[256] = { 0 };	
	memset(arr_unique_simbols, 0, sizeof(int) * 256);
	sym simbols_1[256] = { 0 };
	//посимвольно считываем строку и составляем таблицу встречаемости
	for (int i = 0; i < String.size(); i++)
	{
		//цикл для подсчета информация из строки
		chh = String[i];
		for (int j = 0; j < 256; j++)
		{
			//если символ нашли в массиве записей символов, то в массиве количества уникальных символов увеличиваем количество
			//и увеличиваем общее количество символов
			if (chh == simbols_1[j].ch)
			{
				arr_unique_simbols[j]++;
				couter_of_all_simbols++;
				break;
			}
			//если не нашли в массиве записей символов, то знаносим этот символ 
			//в массиве количества уникальных символов ставим единицу
			//и увеличиваем общее количество символов и уникальных символов
			if (simbols_1[j].ch == 0)
			{
				simbols_1[j].ch = chh;//(unsigned char)chh;
				arr_unique_simbols[j] = 1;
				couter_unique_simbols++;
				couter_of_all_simbols++;
				break;
			}
		}
	}
	// расчет частоты встречаемости
	for (int i = 0; i < couter_unique_simbols; i++)
	{
		simbols_1[i].freq = (float)arr_unique_simbols[i] / couter_of_all_simbols;
	}
	// в массив указателей заносим адреса записей
	for (int i = 0; i < couter_unique_simbols; i++)
	{
		psym[i] = &simbols_1[i];
	}
	//сортировка по убыванию
	sym tempp;
	for (int i = 1; i < couter_unique_simbols; i++)
	{
		for (int j = 0; j < couter_unique_simbols - 1; j++)
		{
			if (simbols_1[j].freq < simbols_1[j + 1].freq)
			{
				tempp = simbols_1[j];
				simbols_1[j] = simbols_1[j + 1];
				simbols_1[j + 1] = tempp;
			}
		}
	}
	//печатаем статистику 
	//по итогу сумма частот должна дать 1
	for (int i = 0; i < couter_unique_simbols; i++)
	{
		simbols[i] = simbols_1[i];
		psym[i] = &simbols[i];
		summ_of_all_freq += simbols[i].freq;
		//printf("Character = %d\tFrequancy = %f\tSymbol = %c\t\n", simbols[i].ch, simbols[i].freq, psym[i]->ch);
		//wprintf(L"Character = %с\tFrequancy = %f\t\n", simbols[i].ch, simbols[i].freq);
		wcout << "Character = " << simbols[i].ch << "\tFrequancy = " << simbols[i].freq << endl;
	}
	//printf("\nKolovo simvolov : %d\nSumm of all Frequancy : %f\n", couter_of_all_simbols, summ_of_all_freq);
	wcout << "\nKolovo simvolov : " << couter_of_all_simbols << "\nSumm of all Frequancy : " << summ_of_all_freq << endl;
}
//функция кодирования строки
void CodeHuffman(wstring String, char* BinaryCode, sym* root)
{
	//char* BinCode = new char[1000000];
	char* BinCode = new char[String.size()*10];
	BinCode[0] = 0;
	Size_Encode = 0;
	for (int i = 0; i < String.size(); i++)
	{
		chh = String[i];
		for (int j = 0; j < couter_unique_simbols; j++)
			if (chh == simbols[j].ch)
			{
				//записываем коды символов из дерева
				//strcat_s(BinCode, _countof(temp), simbols[j].code);
				strcat(BinCode, simbols[j].code);
			}
	}
	//считаем размер закодированной строки
	Size_Encode = strlen(BinCode);
	BinaryCode[0] = 0;
	strcat(BinaryCode, BinCode);
	delete[] BinCode;
}
//функция декодирования строки
void DecodeHuffman(char* BinaryCode, wchar_t* ReducedString, sym* root)
{
	sym* Current;// указатель в дереве
	char CurrentBit;// значение текущего бита кода
	int BitNumber;
	int CurrentSimbol;//индекс распаковываемого символа
	bool FlagOfEnd; //флаг конца битовой последовательности
	FlagOfEnd = false;
	CurrentSimbol = 0;
	BitNumber = 0;
	Current = root;
	//пока не закончилась битовая последовательность
	while (BitNumber != strlen(BinaryCode))
	{
		//пока не пришли в лист дерева
		while (Current->left != NULL && Current->right != NULL && BitNumber != strlen(BinaryCode))
		{
			//читаем значение очередного бита
			CurrentBit = BinaryCode[BitNumber++];
			//бит – 0, то идем налево, бит – 1, то направо
			if (CurrentBit == '0')
				Current = Current->left;
			else
				Current = Current->right;
		}
		//пришли в лист и формируем очередной символ
		ReducedString[CurrentSimbol++] = Current->ch;
		Current = root;
	}
	ReducedString[CurrentSimbol] = 0;
}