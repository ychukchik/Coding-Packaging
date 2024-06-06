#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>

#include "Fano_2.0.h"
#include "LZ.h"

#define MAX_FILE 30

void usage();

/*Требования к файлу декодирования по Фано
- первая строка - это длина зашифрованного текста (бит)
- вторая - это размер алфавита
- третья - это количество символов сжатия, получившихся в ходе кодирования
- предпоследняя строка файла содержит символы сжатия
- последняя содержит биты, которые не влезли в стандартный размер char (эта строка может быть пустой,
  если общее число получившихся бит кратно 8)
- остальные строки начинаются с символа "*" и содержат информацию о кодировке исходных символов
  в такой последовательности: символ (сразу после служебного символа "*"), пробел, знак "-" (минус), пробел, кодировка символа.
  Пример файла декодирования:
50
7
6
*a - 00
*Y - 01
*n - 10
*k - 110
*o - 1110
*c - 11110
*h - 11111
HKЅь%„
10
  */

void CheckArgv(int argc, char** argv);

int main(int argc, char** argv)
{
	
	CheckArgv(argc, argv);
	char c = 0;

	double PCFreq = 0.0;
	__int64 CounterStart = 0;
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
	{
		printf("Error!\n");
		return 0;
	}
	PCFreq = ((double)(li.QuadPart)) / 1000000.0;
	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
	char input[32], output[32];
	while (1)
	{
		printf("\nEnter alg: ");
		scanf("%c", &c);
		switch (c)
		{
		case 'f':
			printf("Input: ");
			scanf("%s", input);
			printf("Output: ");
			scanf("%s", output);
			FanoEncod(input, output);
			//FanoEncod("in.doc", "out.txt");
			//FanoEncod("in1.txt", "out.txt");
			//FanoEncod("in.txt", "out.txt");
			//FanoEncod("1.txt", "out.txt");
			break;
		case 'F':
			printf("Input: ");
			scanf("%s", input);
			printf("Output: ");
			scanf("%s", output);
			FanoDecod(input, output);
			//FanoDecod(*argv++, *++argv);
			//FanoDecod("out.txt", "res.txt");
			//FanoDecod("array_analysis.txt", "res.txt");

			break;
		case 'l':
			printf("Input: ");
			scanf("%s", input);
			printf("Output: ");
			scanf("%s", output);
			LZ78Encod(input, output);
			//LZ78Encod(*argv++, *++argv);
			//LZ78Encod("in.txt", "out.txt");
			//LZ78Encod("in.doc", "out.txt");
			break;
		case 'L':
			printf("Input: ");
			scanf("%s", input);
			printf("Output: ");
			scanf("%s", output);
			LZ78Decod(input, output);
			//LZ78Decod(*argv++, *++argv);
			//LZ78Decod("out.txt", "res.txt");
			break;
		default:
			usage();
			break;
		}
	}

	QueryPerformanceCounter(&li);
	printf("\ntime = %.3lf microseconds\n\n", ((double)(li.QuadPart - CounterStart)) / PCFreq);

	return 0;
}

void CheckArgv(int argc, char** argv)
{
	/*for (int i = 0; i < argc; i++)
	{
		printf("%s\n", argv[i]);
	}*/

	if (argc == 4) {
		if (!(strcmp(argv[1], "f") || strcmp(argv[1], "F") || strcmp(argv[1], "l") || strcmp(argv[1], "L")))
		{
			printf("Enter correct value of algorithm! f, F, l or L!\n");
			exit(-1);
		}
		int i = 0;
		for (; (argv[2][i] != '\0') && (i < MAX_FILE); i++);
		if (!((argv[2][i - 4] == '.' && argv[2][i - 3] == 'd' && argv[2][i - 2] == 'o' && argv[2][i - 1] == 'c') || (argv[2][i - 4] == '.' && argv[2][i - 3] == 't' && argv[2][i - 2] == 'x' && argv[2][i - 1] == 't')))
		{
			printf("Enter correct file! .txt or .doc!\n");
			exit(-1);
		}
		i = 0;
		for (; (argv[3][i] != '\0') && (i < MAX_FILE); i++);
		if (!(argv[3][i - 4] == '.' && argv[3][i - 3] == 't' && argv[3][i - 2] == 'x' && argv[3][i - 1] == 't'))
		{
			printf("Enter correct .txt file!\n");
			exit(-1);
		}
	}
	else {
		printf("Wrong count of argv!\n");
		usage();
		exit(-1);
	}
	return 1;
}

void usage()
{
	printf("\nEnter name of algorithm:\n\
		f - to encode Fano alg.\n\
		F - to decode Fano alg.\n\
		l - to encode LZ alg.\n\
		L - to decode LZ alg.\n\
		Tnen enter two files: .doc or .txt");
}