﻿//#include "stdafx. h"
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
int chh;//переменная для подсчета информация из строки
int k = 0;
//счётчик количества различных букв, уникальных символов
int kk = 0;//счетчик количества всех знаков в файле
int kolvo[256] = { 0 };
//инициализируем массив количества уникальных символов
sym simbols[256] = { 0 };//инициализируем массив записей
sym* psym[256];//инициализируем массив указателей на записи
float summir = 0;//сумма частот встречаемости

int main()
{
	char* String = new char[1000];
	char* BinaryCode = new char[1000];
	char* ReducedString = new char[1000];
	String[0] = BinaryCode[0] = ReducedString[0] = 0;
	//cout << "enter line : ";
	//cin >> String;
	//schitivanie iz file
	FILE* stream;
	errno_t A = fopen_s(&stream, "A.txt", "r");//("A.txt", "r");
	char x;
	int i = 0;
	while ((feof(stream)==0))//(A) == 0))
	{
		fscanf_s(stream, "%c", &x);//(A, "%c", &x);
		String[i] = x;
		i++;
	}
	String[i - 1] = '\0';
	fclose(stream);//(A);
	sym* symbols = new sym[k];
	//создание динамического массива структур simbols
	sym** psum = new sym * [k];
	//создание динамического массива указателей на simbols
	Statistics(String);
	sym* root = makeTree(psym, k);
	//вызов функции создания дерева Хаффмана
	makeCodes(root);//вызов функции получения кода
	CodeHuffman(String, BinaryCode, root);
	/*
	cout << "code : " << endl;
	cout << BinaryCode << endl;
	*/
	//FILE* B = fopen_s("B.txt", "w");
	//fprintf(B, "%s ", BinaryCode);
	//fclose(B);
	errno_t B = fopen_s(&stream, "B.txt", "w");
	fprintf(stream, "%s ", BinaryCode);
	fclose(stream);
	DecodeHuffman(BinaryCode, ReducedString, root);
	/*
	cout << "decode : " << endl;
	cout << ReducedString << endl;
	*/
	/*
	FILE* C = fopen_s("C.txt", "w");
	fprintf(C, "%s ", ReducedString);
	fclose(C);*/
	errno_t C = fopen_s(&stream, "C.txt", "w");
	fprintf(stream, "%s ", ReducedString);
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
	int i, j;
	sym* temp;
	temp = new sym;
	temp->freq = psym[k - 1]->freq + psym[k - 2]->freq;
	temp->code[0] = 0;
	temp->left = psym[k - 1];
	temp->right = psym[k - 2];
	if (k == 2)
		return temp;
	else
	{
		//внесение в нужное место массива элемента дерева Хаффмана
		for (i = 0; i < k; i++)
			if (temp->freq > psym[i]->freq)
			{
				for (j = k - 1; j > i; j--)
					psym[j] = psym[j - 1];
				psym[i] = temp;
				break;
			}
	}
	return makeTree(psym, k - 1);
}

//рекурсивная функция кодирования дерева

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

/*функция подсчета количества каждого символа и его вероятности*/
void Statistics(char* String)
{
	int i, j;
	//побайтно считываем строку и составляем таблицу встречаемости
	for (i = 0; i < strlen(String); i++)
	{
		chh = String[i];
		for (j = 0; j < 256; j++)
		{
			if (chh == simbols[j].ch)
			{
				kolvo[j]++;
				kk++;
				break;
			}
			if (simbols[j].ch == 0)
			{
				simbols[j].ch = (unsigned char)chh;
				kolvo[j] = 1;
				k++; kk++;
				break;
			}
		}
	}
	// расчет частоты встречаемости
	for (i = 0; i < k; i++)
		simbols[i].freq = (float)kolvo[i] / kk;
	// в массив указателей заносим адреса записей
	for (i = 0; i < k; i++)
		psym[i] = &simbols[i];
	//сортировка по убыванию
	sym tempp;
	for (i = 1; i < k; i++)
		for (j = 0; j < k - 1; j++)
			if (simbols[j].freq < simbols[j + 1].freq)
			{
				tempp = simbols[j];
				simbols[j] = simbols[j + 1];
				simbols[j + 1] = tempp;
			}

	for (i = 0; i < k; i++)
	{
		summir += simbols[i].freq;
		printf("Ch= %d\tFreq= %f\tPPP= %c\t\n", simbols[i].ch, simbols[i].freq, psym[i]->ch, i);
	}
	printf("\n Slova = %d\tSummir=%f\n", kk, summir);
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
				strcat_s(BinaryCode, _countof(temp), simbols[j].code);
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