#include "LZ.h"

#define START_SIZE 2
#define PREFIX_SIZE 10
#define MAX_WORD_SIZE 10

int count_of_encode_symbols_for_report = 0;

//enum modes
//{
//	START,
//	INDEX,
//	CHAR
//};

//enum Bool
//{
//	FALSE,
//	TRUE
//};

enum FILESlz
{
	TXT,
	DOC,
	ERROR
};

typedef struct WORD
{
	char* word;
	int index;
	char symbol;
}WORD;

void MemoryError()
{
	perror("Error: no memory!");
	exit(-1);
}
void FileErrorLZ()
{
	perror("Error: file not open!");
	exit(-1);
}

void LZ78Encod(char* file_in, char* file_out)
{
	WORD* dictionary = NULL;//словарь, хранит пару идекс-символ для каждого "слова" и само слово
	dictionary = (WORD*)malloc(sizeof(WORD));
	if (dictionary == NULL) MemoryError();
	dictionary[0].word = (WORD*)malloc(PREFIX_SIZE*sizeof(WORD));
	if (dictionary[0].word == NULL) MemoryError();
	dictionary = Init_word0(dictionary);

	FILE* f_in = fopen(file_in, "rb");
	if (!f_in)FileErrorLZ();
	int len = 0;
	char* txt = NULL;

	enum FILESlz f = ERROR;
	f = WhatFileLZ(file_in);
	if (f == DOC)
	{
		char c_f_in = 0;
		fseek(f_in, 0xa00, SEEK_SET);
		while ((c_f_in = fgetc(f_in)) && (c_f_in != NULL))
		{
			len++;
		}
		txt = (char*)malloc((len) * sizeof(char));
		fseek(f_in, 0xa00, SEEK_SET);
		fread(txt, sizeof(char), len, f_in);
		txt[len - 1] = '\0';
		//printf("%s\n", txt);
	}
	else if (f == TXT)
	{
		fseek(f_in, 0x00, SEEK_END);
		len = ftell(f_in);
		//printf("%d\n", len);
		rewind(f_in);

		txt = (char*)malloc((len + 1) * sizeof(char));//запись из файла в массив
		if (txt == NULL) MemoryError();

		if (fread(txt, sizeof(char), len, f_in) == 0)
		{
			printf("Empty file!\n");
			exit(0);
		}
		txt[len] = '\0';
		//printf("%s\n", txt);
	}
	else
	{
		printf("File is not .txt or .doc!\n");
		exit(-1);
	}

	fclose(f_in);


	char* prefix = (char*)calloc(PREFIX_SIZE, sizeof(char));
	if (prefix == NULL) MemoryError();
	char* character = (char*)calloc(2, sizeof(char));
	if (character == NULL) MemoryError();
	char* tmp = NULL;
	int word_index = -2;
	int index_for_prefix = 1;
	int size_of_dictionary = 0;

	for (int i = 0; i < len; i++)
	{
		//printf("\nIteration %d\n", i);
		character[0] = txt[i];
		character[1] = '\0';
		//printf("txt[%i] = %s\n", i, character);

		tmp = (char*)realloc(tmp, ((PREFIX_SIZE+2) * sizeof(char)) * sizeof(char));
		if (tmp == NULL) MemoryError();

		strcpy(tmp, prefix); 
		strcat(tmp, character); //tmp = prefix + character

		//printf("tmp = %s\n", tmp);

		index_for_prefix = find(tmp, dictionary, size_of_dictionary);
 		if (index_for_prefix != -1)// найдено в словаре
		{			
			strcat(prefix, character);
			//printf("prefix = %s\n", prefix);
			if (len - i == 1) //конец файла, когда остался один символ
			{
				size_of_dictionary++;
				dictionary = (WORD*)realloc(dictionary, (size_of_dictionary + 1) * sizeof(WORD));
				dictionary[size_of_dictionary].word = (WORD*)calloc(PREFIX_SIZE, sizeof(WORD));
				strcpy(dictionary[size_of_dictionary].word, tmp);
				dictionary[size_of_dictionary].index = index_for_prefix;
				dictionary[size_of_dictionary].symbol = '\0';
			}
		}
		else // не найдено, новая запсь в словаре
		{
			if (prefix[0] == '\0')
			{
				word_index = 0;           // префикс пустой, запись с индексом 0
			}
			else
			{
				word_index = find(prefix, dictionary, size_of_dictionary);     // поиск индекса для префикса
				if (word_index == -1)
				{
					word_index = 0;
				}
			}

			size_of_dictionary++;
			dictionary = (WORD*)realloc(dictionary, (size_of_dictionary+1) * sizeof(WORD));
			dictionary[size_of_dictionary].word = (WORD*)calloc(PREFIX_SIZE, sizeof(WORD));
			strcpy(dictionary[size_of_dictionary].word, tmp);//размеры..

			//printf("dictionary[%d].word = %s\n", size_of_dictionary, dictionary[size_of_dictionary].word);
			dictionary[size_of_dictionary].index = word_index;
			//printf("txt[%i] = %c\n", i, txt[i]);
			dictionary[size_of_dictionary].symbol = txt[i];
			//printf("dictionary[%d].symbol = %c\n", size_of_dictionary, dictionary[size_of_dictionary].symbol);
			//prefix[0] = '\0';
			memset(prefix, 0x00, PREFIX_SIZE);
			memset(tmp, 0x00, PREFIX_SIZE+2);
			character[0] = '\0';
		}
	}

	free(txt);
	free(prefix);
	free(character);
	free(tmp);

	WriteToFile(file_out, dictionary, size_of_dictionary);
	FreeWords(dictionary, size_of_dictionary);
	free(dictionary);

	printf("\nCompression = %lf", (len / (double)(count_of_encode_symbols_for_report)));
}

void LZ78Decod(char* file_out, char* file_res)
{
	FILE* f_out = fopen(file_out, "rb");
	if (!f_out)FileErrorLZ();

	fseek(f_out, 0x00, SEEK_END);
	int len = ftell(f_out);
	//printf("%d\n", len);
	rewind(f_out);

	char* encode_str = (char*)malloc((len + 1) * sizeof(char));//запись из файла в массив
	if (encode_str == NULL) MemoryError();

	if (fread(encode_str, sizeof(char), len, f_out) == 0)
	{
		printf("Empty file!\n");
		exit(0);
	}
	encode_str[len] = '\0';
	//printf("%s\n", encode_str);
	fclose(f_out);

	WORD* dictionary = NULL;
	dictionary = (WORD*)malloc(sizeof(WORD));
	if (dictionary == NULL) MemoryError();
	dictionary[0].word = (WORD*)malloc(PREFIX_SIZE * sizeof(WORD));
	if (dictionary[0].word == NULL) MemoryError();
	dictionary = Init_word0(dictionary);

	int size_of_dictionary = 0;
	char sym;
	//int num_digit = 0; //индикатор разряда числа
	//enum modes mode = START;

	for (int i = 0; i < len; i++)
	{
		/*if (mode == START)
		{
			mode = INDEX;
			size_of_dictionary++;
			dictionary = (WORD*)realloc(dictionary, (size_of_dictionary + 1) * sizeof(WORD));
			dictionary[size_of_dictionary].word = (WORD*)calloc(PREFIX_SIZE, sizeof(WORD));
			dictionary[size_of_dictionary].index = atoi(encode_str[i]);
			i++;
			dictionary[size_of_dictionary].word[0] = encode_str[i];
			dictionary[size_of_dictionary].symbol = encode_str[i];
		}*/
		//else if (mode == INDEX/* && (isdigit(encode_str[i])*/)
		//{
		size_of_dictionary++;
		dictionary = (WORD*)realloc(dictionary, (size_of_dictionary + 1) * sizeof(WORD));
		if (dictionary == NULL) MemoryError();
		dictionary[size_of_dictionary].word = (WORD*)calloc(PREFIX_SIZE, sizeof(WORD));
		if (dictionary[size_of_dictionary].word == NULL) MemoryError();

		if (size_of_dictionary < 10)
		{
			//printf("%d\n", encode_str[i] - '0');
			dictionary[size_of_dictionary].index = encode_str[i] - '0';
			i++;
			dictionary[size_of_dictionary].symbol = encode_str[i];
			SearchForWord(dictionary, dictionary[size_of_dictionary].index, size_of_dictionary, encode_str, i); //сохранение слова в dictionary[size_of_dictionary].word
		}
		else if (size_of_dictionary < 100)
		{
			dictionary[size_of_dictionary].index = encode_str[i] - '0';
			i++;
			if (encode_str[i] < '0' || encode_str[i] > '9') //следующий символ - буква. Значит, разряд == 1
			{
				dictionary[size_of_dictionary].symbol = encode_str[i];
				SearchForWord(dictionary, dictionary[size_of_dictionary].index, size_of_dictionary, encode_str, i);
			}
			else  //следующий символ - цифра. Значит, разряд == 2
			{
				dictionary[size_of_dictionary].index = 10 * dictionary[size_of_dictionary].index + (encode_str[i] - '0');
				i++;
				dictionary[size_of_dictionary].symbol = encode_str[i];
				SearchForWord(dictionary, dictionary[size_of_dictionary].index, size_of_dictionary, encode_str, i);
			}
		}
		else if (size_of_dictionary < 1000)
		{
			dictionary[size_of_dictionary].index = encode_str[i] - '0';
			i++;
			if (encode_str[i] < '0' || encode_str[i] > '9') //следующий символ - буква. Значит, разряд == 1
			{
				dictionary[size_of_dictionary].symbol = encode_str[i];
				SearchForWord(dictionary, dictionary[size_of_dictionary].index, size_of_dictionary, encode_str, i);
			}
			else if (encode_str[i] >= '0' && encode_str[i] <= '9' && (encode_str[i+1] < '0' || encode_str[i+1] > '9')) //следующий символ - цифра, а еще следующий - буква. Значит, разряд == 2
			{
				dictionary[size_of_dictionary].index = 10 * dictionary[size_of_dictionary].index + (encode_str[i] - '0');
				i++;
				dictionary[size_of_dictionary].symbol = encode_str[i];
				SearchForWord(dictionary, dictionary[size_of_dictionary].index, size_of_dictionary, encode_str, i);
			}
			else //следующий символ - цифра, и еще следующий - цифра. Значит, разряд == 3
			{
				dictionary[size_of_dictionary].index = 10 * dictionary[size_of_dictionary].index + (encode_str[i] - '0');
				i++;
				dictionary[size_of_dictionary].index = 10 * dictionary[size_of_dictionary].index + (encode_str[i] - '0');
				i++;
				dictionary[size_of_dictionary].symbol = encode_str[i];
				SearchForWord(dictionary, dictionary[size_of_dictionary].index, size_of_dictionary, encode_str, i);
			}
		}

			//else if (size_of_dictionary < 100)
			//{	
			//	dictionary[size_of_dictionary].index = atoi(encode_str[i]);
			//	if (encode_str[i + 1] >= '0' && encode_str[i + 1] <= '9' && ((i+2>= len) || (encode_str[i + 2] <= '0' && encode_str[i + 2] >= '9'))) //значит двузначное
			//	{
			//		if (i + 2 >= len) //если мы в конце строки
			//		{
			//			i++;
			//			dictionary[size_of_dictionary].symbol = encode_str[i];
			//		}
			//		i++;
			//		dictionary[size_of_dictionary].index = 10 * dictionary[size_of_dictionary].index + atoi(encode_str[i]);
			//	}
			//	
			//}
			//else if (size_of_dictionary <= 100)
			//{
			//	dictionary[size_of_dictionary].index = atoi(encode_str[i]);
			//	if (encode_str[i + 1] >= '0' && encode_str[i + 1] <= '9' && ((i + 2 >= len) || (encode_str[i + 2] <= '0' && encode_str[i + 2] >= '9'))) //значит двузначное
			//	{
			//		i++;
			//		dictionary[size_of_dictionary].index = 10 * dictionary[size_of_dictionary].index + atoi(encode_str[i]);
			//		
			//	}
			//	else if(encode_str[i + 1] >= '0' && encode_str[i + 1] <= '9' && ((i + 2 >= len) || (encode_str[i + 2] >= '0' && encode_str[i + 2] <= '9')) && ((i + 3 >= len) || (encode_str[i + 3] <= '0' && encode_str[i + 3] >= '9')))
			//}
			//else if (size_of_dictionary < 1000)
			//{
			//}
	//		}
	//	}
	//	else if (mode == CHAR)
	//	{

		//}
	}
	WriteAnswer(dictionary, size_of_dictionary, file_res);

	free(encode_str);
	FreeWords(dictionary, size_of_dictionary);
	free(dictionary);
}

void WriteAnswer(WORD* Dictionary, int size_of_dictionary, char* file_res)
{
	FILE* f_res = fopen(file_res, "w");
	if (!f_res)FileErrorLZ();

	for (int i = 1; i <= size_of_dictionary; i++)
	{
		fprintf(f_res, "%s", Dictionary[i].word);
		//printf("%s", Dictionary[i].word);
	}

	fclose(f_res);
}
                                   //dictionary[size_of_dictionary].index
void SearchForWord(WORD* Dictionary, int ind, int size_of_dictionary, char* encode_str, int i)
{
	if (ind == 0)
	{
		Dictionary[size_of_dictionary].word[0] = encode_str[i];
		Dictionary[size_of_dictionary].word[1] = '\0';

		//printf("dictionary[%d].word (ind = 0) = %s\n", size_of_dictionary, dictionary[size_of_dictionary].word);
	}
	else 
	{
		//printf("dictionary[%d].word (new) = %s\n", size_of_dictionary, dictionary[size_of_dictionary].word);
		//printf("dictionary[%d].word (old) = %s\n", ind, dictionary[size_of_dictionary].word);
		strcpy(Dictionary[size_of_dictionary].word, Dictionary[ind].word);
		//printf("dictionary[%d].word (res) = %s\n", size_of_dictionary, dictionary[size_of_dictionary].word);
		//printf(" dictionary[size_of_dictionary].symbol = %c", dictionary[size_of_dictionary].symbol);
		char* a = (char*)malloc(2 * sizeof(char));
		a[0] = Dictionary[size_of_dictionary].symbol;
		a[1] = '\0';
		strcat(Dictionary[size_of_dictionary].word, a/*dictionary[size_of_dictionary].symbol*/);
		//printf("dictionary[%d].word (res) = %s\n", size_of_dictionary, dictionary[size_of_dictionary].word);
		free(a);
	}
}
	
		

void FreeWords(WORD* Dictionary, int size_of_dictionary)
{
	for (int i = 1; i <= size_of_dictionary; i++)
	{
		free(Dictionary[i].word);
	}
}

void WriteToFile(char* file_out, WORD* Dictionary, int size_of_dictionary)
{
	FILE* f_out = fopen(file_out, "w");
	if (!f_out)FileErrorLZ();

	for (int i = 1; i <= size_of_dictionary; i++)
	{
		fprintf(f_out, "%d%c", Dictionary[i].index, Dictionary[i].symbol);
		count_of_encode_symbols_for_report += 2;
		if (Dictionary[i].index >= 10)count_of_encode_symbols_for_report++;
		if (Dictionary[i].index >= 100)count_of_encode_symbols_for_report++;
		//printf("%d%c  ", Dictionary[i].index, Dictionary[i].symbol);
	}
	fclose(f_out);
}
        //prefix+character
int find(char* tmp, WORD* Dictionary, int size_of_dictionary)
{ 
	int i = size_of_dictionary;
	while (i)
	{
		//printf("dictionary[%i].word = %s\n", i, dictionary[i].word);
		if (strcmp(tmp, Dictionary[i].word)==0)
		{
			return i;
		}
		i--;
	}
	return -1;
}

WORD* Init_word0(WORD* dictionary)
{
	dictionary[0].index = 0;
	dictionary[0].word[0] = '\0';
	dictionary[0].symbol = '\0';
	return dictionary;
}

int WhatFileLZ(char* in)
{
	int i = 0;
	for (; (in[i] != '\0') && (i < 30); i++);

	if (in[i - 3] == 'd' && in[i - 2] == 'o' && in[i - 1] == 'c')
		return DOC;
	else if (in[i - 3] == 't' && in[i - 2] == 'x' && in[i - 1] == 't')
		return TXT;
	else return ERROR;
}

//int SymbolToDictionary(char buf, WORD* dictionary, int* size_of_dictionary, char* work_str, int* index_work_str)
//{
//	if (*size_of_dictionary == 1)
//	{
//		*size_of_dictionary++;
//		dictionary = (WORD*)realloc(dictionary, (*size_of_dictionary + 1) * sizeof(WORD));
//		if (dictionary == NULL) MemoryError();
//
//		char* work_str = (char*)realloc(work_str, (*index_work_str + 3) * sizeof(char));
//		if (dictionary == NULL) MemoryError();
//
//		dictionary[1].index = 1;
//		dictionary[1].symbol = buf;
//		work_str[0] = 0;
//		*index_work_str++;
//		work_str[1] = dictionary[1].symbol;
//		*index_work_str++;
//		work_str[2] = '\0';
//		return FALSE;
//	}
//
//	int is_repeat = FALSE;
//
//	for (int i = 1; i < *size_of_dictionary; i++)
//	{
//		if (dictionary[i].symbol == buf)
//		{
//			//is_repeat = TRUE;
//
//		}
//	}
//	if (is_repeat == FALSE)
//	{
//		*size_of_dictionary++;
//		dictionary = (WORD*)realloc(dictionary, (*size_of_dictionary + 1) * sizeof(WORD));
//		if (dictionary == NULL) MemoryError();
//
//		char* work_str = (char*)realloc(work_str, (*index_work_str + 3) * sizeof(char));
//		if (dictionary == NULL) MemoryError();
//
//		dictionary[*size_of_dictionary].index = *size_of_dictionary;
//		dictionary[*size_of_dictionary].symbol = buf;
//		work_str[*index_work_str] = 0;
//		*index_work_str++;
//		work_str[*index_work_str] = dictionary[1].symbol;
//		*index_work_str++;
//		work_str[*index_work_str] = '\0';
//	}
//
//	return is_repeat;
//}