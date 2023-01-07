#include <iostream>
#include <errno.h>

using namespace std;

struct sym
{
	unsigned char ch;
	float freq;
	char code[255];
	sym* left;
	sym* right;
};

void Statistics(char* String);
sym* makeTree(sym* psym[], int k);
void makeCodes(sym* root);
void CodeHuffman(char* String, char* BinaryCode, sym* root);
void DecodeHuffman(char* BinaryCode, char* ReducedString, sym* root);
int chh;					//переменная для подсчета информация из строки
int k = 0;					//счётчик количества различных букв, уникальных символов
int kk = 0;					//счетчик количества всех знаков в файле
int kolvo[256] = { 0 };		//инициализируем массив количества уникальных символов
sym simbols[256] = { 0 };	//инициализируем массив записей
sym* psym[256];				//инициализируем массив указателей на записи
float summ_of_all_freq = 0;	//сумма частот встречаемости
float Size_Encode = 0;		//сумма в битах сжатой строки
float сompression_ratio = 0;//коэффицент сжатия строки

int main()
{
	char* String = new char[1000];
	char* BinaryCode = new char[1000];
	char* ReducedString = new char[1000];
	String[0] = BinaryCode[0] = ReducedString[0] = 0;
	//schitivanie iz file
	FILE* stream;
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

	sym* symbols = new sym[k];				//создание динамического массива структур simbols
	sym** psum = new sym * [k];				//создание динамического массива указателей на simbols
	Statistics(String);						//вызов функции определения частоты символов в строке
	//если количество уникальных символов больше 2
	if (k >= 2)
	{
		sym* root = makeTree(psym, k);			//вызов функции создания дерева Хаффмана
		makeCodes(root);						//вызов функции получения кода
		CodeHuffman(String, BinaryCode, root);	//кодирование исходной строки по дереву

		cout << "Razmer ishodnogo file :\t" << kk * 8 << " bit\n";
		cout << "Razmer Encode file : \t" << Size_Encode << " bit\n";
		сompression_ratio = ((kk * 8 - Size_Encode) / (kk * 8)) * 100;
		cout << "Compression_ratio : \t" << сompression_ratio << "%\n";

		DecodeHuffman(BinaryCode, ReducedString, root);

		errno_t Output = fopen_s(&stream, "Output.txt", "w");
		fprintf(stream, "Binary Code:\n%s\n", BinaryCode);

		//char temp = 1001;
		//fprintf(stream, "TEST Govna = %c", temp);
		int count = 0;
		fprintf(stream, "Encoding Code = ");
		while (count < strlen(BinaryCode))
		{
			int temp = (BinaryCode[count++] - 48) * 10000000;
			temp += (BinaryCode[count++] - 48) * 1000000;
			temp += (BinaryCode[count++] - 48) * 100000;
			temp += (BinaryCode[count++] - 48) * 10000;
			temp += (BinaryCode[count++] - 48) * 1000;
			temp += (BinaryCode[count++] - 48) * 100;
			temp += (BinaryCode[count++] - 48) * 10;
			temp += (BinaryCode[count++] - 48);
			fprintf(stream, "%c", temp);
		}

		fprintf(stream, "\nDecoding string:\n%s\n", ReducedString);
		fprintf(stream, "Compression ratio file = %f%%\n", сompression_ratio);
	}
	//если строка из одного уникального символа
	else
	{
		errno_t Output = fopen_s(&stream, "Output.txt", "w");
		fprintf(stream, "Binary Code:\n");
		for (int i = 0; i < kk; i++)
		{
			fprintf(stream, "%d", 1);
		}

		fprintf(stream, "Encoding Code = ");
		int count = kk / 8;
		while (count > 0)
		{
			int temp = 11111111;
			count--;
			fprintf(stream, "%c", temp);
		}
		count = kk % 8;
		int temp = 1;
		while (count > 1)
		{
			temp = temp * 10 + 1;
			count--;
		}
		fprintf(stream, "%c", temp);

		fprintf(stream, "\nDecoding string:\n%s\n", String);
		сompression_ratio = (((float)kk * 8 - kk) / (kk * 8)) * 100;
		fprintf(stream, "Compression ratio file = %f%%\n", сompression_ratio);
	}
	fclose(stream);
	delete[] psum;
	delete[] String;
	delete[] BinaryCode;
	delete[] ReducedString;
	return 0;
}

//рeкурсивная функция создания дерева Хаффмана
sym* makeTree(sym* psym[], int k)
{
	sym* temp;
	temp = new sym;
	temp->freq = psym[k - 1]->freq + psym[k - 2]->freq;
	temp->code[0] = 0;
	temp->left = psym[k - 1];
	temp->right = psym[k - 2];
	if (k == 2)
	{
		return temp;
	}
	else
	{
		//внесение в нужное место массива элемента дерева Хаффмана
		for (int i = 0; i < k; i++)
		{
			if (temp->freq > psym[i]->freq)
			{
				for (int j = k - 1; j > i; j--)
				{
					psym[j] = psym[j - 1];
				}	
				psym[i] = temp;
				break;
			}
		}	
	}
	return makeTree(psym, k - 1);
}

//рекурсивная функция кодирования дерева
//которая имитирует кодирование символов инпут файла 
// по дереву, путем формирования символов 0 или 1(вместо битов)
// подсчитывая и возвращая их количество
//влево от узла устанавливаем 0, вправо 1
void makeCodes(sym* root)
{
	if (root->left)
	{
		strcpy_s(root->left->code, root->code);
		strcat_s(root->left->code, "0");
		makeCodes(root->left);
	}
	if (root->right)
	{
		strcpy_s(root->right->code, root->code);
		strcat_s(root->right->code, "1");
		makeCodes(root->right);
	}
}

//функция подсчета количества каждого символа и его вероятности в исходном файле
/*
//вычисение частоты символов в строке
void String::symbol_frequency()
{
    char* temp_1 = new char[len];   //содержит символы
    int* temp_2 = new int[len] {0}; //содержит частоту символа
    //ищем элемент строки в массиве темп1 увеличивавем значение темп2 с индексом вхождения
    //если не нашли добавляем элемент в темп1 и увеличиваем темп2
    for (int i = 0; i < len; i++)
    {
        bool flag = false;
        for (int j = 0; j < len; j++)
        {
            if (str[i] == temp_1[j])
            {
                temp_2[j]++;
                flag = true;
                break;
            }
        }
        if (flag == false)
        {
            int k = 0;
            while (temp_2[k] != 0)
                k++;
            temp_1[k] = str[i];
            temp_2[k]++;
        }
    }
}
*/
void Statistics(char* String)
{
	//посимвольно считываем строку и составляем таблицу встречаемости
	for (int i = 0; i < strlen(String); i++)
	{
		//цикл для подсчета информация из строки
		chh = String[i];
		for (int j = 0; j < 256; j++)
		{
			//если символ нашли в массиве записей символов, то в массиве количества уникальных символов увеличиваем количество
			//и увеличиваем общее количество символов
			if (chh == simbols[j].ch)
			{
				kolvo[j]++;
				kk++;
				break;
			}
			//если не нашли в массиве записей символов, то знаносим этот символ 
			//в массиве количества уникальных символов ставим единицу
			//и увеличиваем общее количество символов и уникальных символов
			if (simbols[j].ch == 0)
			{
				simbols[j].ch = (unsigned char)chh;
				kolvo[j] = 1;
				k++; 
				kk++;
				break;
			}
		}
	}
	// расчет частоты встречаемости
	for (int i = 0; i < k; i++)
	{
		simbols[i].freq = (float)kolvo[i] / kk;
	}	
	// в массив указателей заносим адреса записей
	for (int i = 0; i < k; i++)
	{
		psym[i] = &simbols[i];
	}
	//сортировка по убыванию
	sym tempp;
	for (int i = 1; i < k; i++)
	{
		for (int j = 0; j < k - 1; j++)
		{
			if (simbols[j].freq < simbols[j + 1].freq)
			{
				tempp = simbols[j];
				simbols[j] = simbols[j + 1];
				simbols[j + 1] = tempp;
			}
		}
	}	
	//печатаем статистику 
	//по итогу сумма частот должна дать 1
	for (int i = 0; i < k; i++)
	{
		summ_of_all_freq += simbols[i].freq;
		printf("Character = %d\tFrequancy = %f\tSymbol = %c\t\n", simbols[i].ch, simbols[i].freq, psym[i]->ch);
	}
	printf("\nKolovo simvolov : %d\nSumm of all Frequancy : %f\n", kk, summ_of_all_freq);
}
//функция кодирования строки
void CodeHuffman(char* String, char* BinaryCode, sym* root)
{
	for (int i = 0; i < strlen(String); i++)
	{
		chh = String[i];
		for (int j = 0; j < k; j++)
			if (chh == simbols[j].ch)
			{
				char temp[1000];
				*temp = *BinaryCode;
				//strcat_s(BinaryCode, simbols[j].code);
				//записываем коды символов из дерева
				strcat_s(BinaryCode, _countof(temp), simbols[j].code);
				//считаем размер закодированной строки
				Size_Encode = Size_Encode + (strlen(simbols[j].code)); //* kolvo[j]);
			}
	}
}
//функция декодирования строки
void DecodeHuffman(char* BinaryCode, char* ReducedString, sym* root)
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