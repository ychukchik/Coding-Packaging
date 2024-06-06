#include "Fano_2.0.h"

#define BYTE 8
#define NOTFOUND -1 
#define MAX_FILE 30

enum FILES
{
	TEXT,
	BIN,
	EXE,
	ERROR
};

enum COINCIDENCE
{
	NO,
	YES
};

typedef struct SymbolData
{
	char symbol;
	int repetitions;
	char bin_code[BYTE*2];
} SymbolData;

void FanoDecod(char* file_out, char* file_res)
{
	//printf("file_in %s\tfile_out %s\n", file_out, file_res);
	int len_bin_text = 0;//��������� �� �����
	int alph_size = 0;//��������� �� �����
	int count_of_sym_compression = 0;//��������� �� �����
	FILE* out = fopen(file_out, "r");
	if (!out)FileError();

	fscanf(out, "%d\n", &len_bin_text); //������ �� ����� ����� �������� �������
	fscanf(out, "%d\n", &alph_size); //������ ��������
	fscanf(out, "%d\n", &count_of_sym_compression); //���������� ��������, ������� ���������� � ���� ������
	if (alph_size == 1)
	{
		fclose(out);
		OneSymbolDecod(file_out, file_res);
		return;
	}

	SymbolData* alph = (SymbolData*)calloc(alph_size,sizeof(SymbolData));
	char c;
	//������ ������ "������ - ���"
	for (int i = 0; i < alph_size; i++)
	{
		fscanf(out, "*%c - %s\n", &c, (&alph[i])->bin_code);
		(alph + i)->symbol = c;
		//printf("%c -> %s\n", c, (&alph[i])->bin_code);
	}
	//printf("%d\n", count_of_sym_compression);
	char* symbols_from_file = (char*)malloc(sizeof(char));
	char* tmp = NULL;
	int symbols_for_compression = 0;
	//������ �������� (��� 0 � 1)
	if (len_bin_text >= BYTE)
	{
		while (/*(!feof(out))*/ /*&& (c = fgetc(out)) && (c != '\n') && (c != -1)*/ (count_of_sym_compression>0))
		{
			count_of_sym_compression--;
			c = fgetc(out);
			symbols_for_compression++;
			tmp = (char*)realloc(symbols_from_file, symbols_for_compression * sizeof(char) + 1);
			symbols_from_file = tmp;
			symbols_from_file[symbols_for_compression - 1] = c;
		}
	}
	//printf("compression symbols were read\n");
	while (c != '\n')
	{
		c = fgetc(out);
	}
	
	//������ 0 � 1 (����, ������� �� ������ � ������ �������)
	int not_fit = 0;///////////////////////////////////////////////////////�� ������ exe �� ��������, �� ������ ����, ������ � ��������
	while ((!feof(out)) /*&& (c = fgetc(out)) && (c != -1)*//*&&((c=='0') || (c == '1'))*/)
	{
		c = fgetc(out);
		if ((c == '0') || (c == '1'))
		{
			++not_fit;
			tmp = (char*)realloc(symbols_from_file, (symbols_for_compression + not_fit) * sizeof(char) + 1);
			if (tmp == NULL) printf("no memory!");
			symbols_from_file = tmp;
			symbols_from_file[symbols_for_compression + not_fit - 1] = c;
		}
		//else printf("error\n");
	}
	//printf("free bits were read\nnot_fit == %d\n", not_fit);
	symbols_from_file[symbols_for_compression + not_fit] = '\0';
	//printf("%s\n", symbols_from_file);
	fclose(out);

	
	//int len_bin_text = symbols_for_compression * BYTE + not_fit; //����� ��������� ����, ������� ����������� ������
	int index_in_symbol_array = 0;
	int* bin_text_from_file = (int*)calloc(len_bin_text + 1, sizeof(int));
	if (bin_text_from_file == NULL) printf("no memory!");
	int index_in_bin_text = 0;
	int tmp2;
	unsigned char x;
	for (; index_in_symbol_array < symbols_for_compression;++index_in_symbol_array)
	{
		index_in_bin_text = BYTE * (index_in_symbol_array + 1) - 1;
		x = symbols_from_file[index_in_symbol_array];
		DecToBin(x, bin_text_from_file, &index_in_bin_text, -1);
	}


	if(index_in_bin_text!=0) index_in_bin_text++;
	//������ 0 � 1 � �������� ������
	//int not_fit = len_bin_text - symbols_for_compression * BYTE; //������� � ������, ������� �� ������ � �������
	for (; index_in_bin_text < len_bin_text;++index_in_bin_text)
	{
		bin_text_from_file[index_in_bin_text] = symbols_from_file[index_in_symbol_array] - '0';
		++index_in_symbol_array;
	}


	//for (int e = 0; e < len_bin_text;e++) //�������
	//{
	//	printf("%d ", bin_text_from_file[e]);
	//}


	//�������� ������������ � ������ ���������� � ����
	FILE* res = fopen(file_res, "w");
	if (!res)FileError();

	int symbol_index;
	int count_of_bin;
	enum COINCIDENCE flag_coincidence;
	int readed = 0;
	while (readed < len_bin_text)
	{
		symbol_index = 0;//����� ������ �� ��������
		count_of_bin = 0;//���-�� ����� � ���� �������
		flag_coincidence = YES;//����������/������������
		while (flag_coincidence == YES)
		{
			//�������, ������� ����� �������� ��� �������
			for (count_of_bin = 0;(symbol_index < alph_size) && (count_of_bin < BYTE) && (alph[symbol_index].bin_code[count_of_bin] != '\0'); count_of_bin++);
			for (int q = 0; q < count_of_bin; q++)
			{
				if ((alph[symbol_index].bin_code[q] - '0') != bin_text_from_file[readed + q])
					//������������
				{
					flag_coincidence = NO;
					break;
				}
			}
			if (flag_coincidence == YES) break;
			else
			{
				symbol_index++;
				flag_coincidence = YES;
			}
		}
		if (flag_coincidence == YES)
		{
			fprintf(res, "%c", alph[symbol_index].symbol);
			readed += count_of_bin;
		}
	}

	free(symbols_from_file);
}

void FanoEncod(char* file_in, char* file_out)
{
	//printf("file_in %s\tfile_out %s\n", file_in, file_out);
	int text_size_for_report = 0;
	enum FILES f1 = ERROR, f2 = ERROR;  //��� �������� ���������� �����
	int count_of_name = 0; //��� �������� ������ ���������� ����� �������� �����, ����� ��� ���������� ��������� �� ������� ����������
	while (file_in[count_of_name] != '\0') //������� �������� � ����� �����
	{
		++count_of_name;
	}
	char* new_name = (char*)malloc(count_of_name * sizeof(char));
	if (new_name == NULL) printf("no memory!");
	strcpy(new_name, file_in);//�������� �������� �����

	//����� ����� ���� ������ ����������� �� ����������.
	//���� ������ ������ ���� doc, ����� ���������� ��� � ���������. ��������� �������� ������ �������� � ������
	f1 = WhatFile(new_name);
	f2 = WhatFile(file_out);
	int textSize = 0;
	if (f1 == TEXT && f2 == TEXT)
	{
		//printf("TXT\n"); ////��� �������
		textSize = TXTSize(new_name);
	}
	else if (f1 == BIN && f2 == TEXT)
	{
		//printf("DOC\n"); /// ��� �������
		textSize = DocToText(new_name); //����������� ������ �� doc � txt
		//printf("qwer\n");
	}
	else if (f1 == EXE && f2 == TEXT)
	{
		//printf("EXE\n"); /// ��� �������
		textSize = ExeToText(new_name); //����������� ������ �� exe � txt
		//rename(file_in, "1.txt");
		//if (rename(file_in, "1.txt") == 0)            // �������������� �����
		//	printf( "File was renamed. Good\n");
		//else
		//{
		//	printf("Invalid file name!\n");
		//	exit(-1);
		//}
		//
		//strcpy(new_name, "1.txt");
		//printf("okey\n");
	}
	else
	{
		printf("Invalid file name!\n");
		exit(-1);
	}
	text_size_for_report = textSize;

	//�������� �����
	FILE* f_in = fopen(new_name, "r");
	if (!f_in)FileError();

	//���������� � ������ �������� �����
	int text_size = 0;
	int alph_size = 0;
	SymbolData* alph = (SymbolData*)malloc(sizeof(SymbolData));
	SymbolData* tmp = NULL;
	char c = 0;
	int k = 0;
	//������ ��������� ����� � �������� ��������
	while ((!feof(f_in)) /*&& (c = fgetc(f_in)) && (c != EOF)*/&&(textSize!=0))
	{
		textSize--;
		text_size++;
		c = fgetc(f_in);

		k = StructStrstr(c, alph, alph_size);

		if (k != NOTFOUND)//������ "�" ��� ���� � ��������, ��������� ������� � �������
		{
			alph[k].repetitions++;
		}
		else//������ ������� ��� �� ����, ���������� � �������
		{
			alph_size++;
			if ((tmp = (SymbolData*)realloc(alph, alph_size * sizeof(SymbolData))))//�������� ����� ��� ��� ������ �������
			{
				alph = tmp;
				alph[alph_size - 1].symbol = c;
				alph[alph_size - 1].repetitions = 1;
			}
			else
			{
				perror("Error! No memory!\n");
				exit(-1);
			}
		}
	}

	//printf("text_size = %d\n", text_size);
	//printf("alph_size = %d\n", alph_size);
	if (alph_size == 0)
	{
		printf("Error! No text!");
		exit(-1);
	}
	if (alph_size == 1)//��������� ��������� ��� ������ �������!
	{
		OneSymbolEncod(file_out, alph);
		return;
	}



	//���������� ���� �� �������� ����������
	Sorting(alph, alph_size);
	//����������� ��������
	MakeBinCode(alph, alph_size, 0);
	//������ �������� � ���� ������� �������
	//PrintData(alph, alph_size);

	//���������� � ����������� ������
	int* bin_text = (int*)malloc(sizeof(int));
	int len_bin_text = 0;

	f_in = fopen(new_name, "r");
	if (!f_in)FileError();

	int* tmpB = NULL;
	int len_sym_bc = 0;
	//�����������
	while ((!feof(f_in)) && (text_size != 0) /*&& (c = fgetc(f_in)) && (c != -1)*/)
	{
		text_size--;
		c = fgetc(f_in);
		len_sym_bc = 0;
		for (int i = 0; i < alph_size; i++)
		{
			if (c == alph[i].symbol)
			{
				for (int q = 0; alph[i].bin_code[q] != '\0'; q++)
				{
					len_sym_bc++;
				}
				len_bin_text += len_sym_bc;
				if (tmpB = (int*)realloc(bin_text, len_bin_text * sizeof(int)))
				{
					bin_text = tmpB;
					for (int k = 0; k < len_sym_bc; k++)
					{
						bin_text[len_bin_text - len_sym_bc + k] = alph[i].bin_code[k] - '0';
					}
					break;
				}
				else
				{
					perror("Error! No memory!\n");
					exit(-1);
				}
			}
		}
	}
	fclose(f_in);


	//��� �������
	/*for (int i = 0; i < len_bin_text; i++)
	{
		printf("%d ", bin_text[i]);
	}*/
	//printf("\n%d\n", len_bin_text);
	


	//������ �������� ��� �������������
	FILE* out = fopen(file_out, "w");
	if (out == NULL)FileError();
	fprintf(out,"%d\n%d\n%d\n", len_bin_text, alph_size, len_bin_text / BYTE);//����� �������������� ������ � ������ ��������
	for (int i = 0; i < alph_size; i++)
	{
		fprintf(out, "*%c - %s\n", alph[i].symbol, alph[i].bin_code);
	}

	fclose(out);
	
	int symbols_for_compression = 0;
	int not_fit = 0;
	PrintBinToFile(file_out, bin_text, len_bin_text, &symbols_for_compression, &not_fit);

	free(alph);
	free(bin_text);


	printf("\nCompression = %lf", (text_size_for_report / (double)(symbols_for_compression + not_fit)));

	//printf("finish!\n");
}

void OneSymbolEncod(char* file_out, SymbolData* alph)
{
	alph[0].bin_code[0] = '0';
	alph[0].bin_code[1] = '\0';
	FILE* out = fopen(file_out, "w");
	if (out == NULL)FileError();
	fprintf(out, "%d\n%d\n", alph->repetitions, 1);//����� �������������� ������ � ������ ��������
	for (int i = 0; i < 1; i++)
	{
		fprintf(out, "*%c - %s\n", alph[i].symbol, alph[i].bin_code);
	}
	//������� ����� ����� �����
	int count_symbols = alph->repetitions / BYTE;

	for (int i = 0; i < count_symbols; i++)
	{
		fprintf(out, "%d", 0);
	}
	fprintf(out,"\n");
	//������� ����� ������ �����
	int not_fit = alph->repetitions - count_symbols * BYTE;
	for (int i = 0; i < not_fit; i++)
	{
		fprintf(out, "%d", 0);
	}

	fclose(out);

}

void OneSymbolDecod(char* file_out, char* file_res)
{
	int len_bin_text = 0;//��������� �� �����
	int alph_size = 0;//��������� �� �����
	FILE* out = fopen(file_out, "r");
	if (!out)FileError();
	FILE* res = fopen(file_res, "w");
	if (!out)FileError();

	fscanf(out, "%d\n", &len_bin_text);
	fscanf(out, "%d\n", &alph_size);

	SymbolData* alph = (SymbolData*)calloc(alph_size, sizeof(SymbolData));
	char c;
	//������ ������ "������ - ���"
	fscanf(out, "*%c - %s\n", &c, (&alph[0])->bin_code);
	alph->symbol = c;
	//printf("%c -> %s\n", c, (&alph[0])->bin_code);
	fclose(out);

	for (int i = 0; i < len_bin_text;i++)
	{
		fprintf(res, "%c", c);
	}
	fclose(res);
	free(alph);
}

int StructStrstr(char c, SymbolData* alph, int alph_size)
{
	if (alph_size == 0) return NOTFOUND;
	for (int i = 0; i < alph_size; i++)
	{
		if (alph[i].symbol == c)
		{
			return i;
		}
	}
	return NOTFOUND;
}

void Sorting(SymbolData* alph, int n) //�� ��������
{
	SymbolData tmp;
	for (int i = 0; i < n - 1; i++)
	{
		for (int j = 0; j < n - i - 1; j++)
		{
			if (alph[j].repetitions < alph[j + 1].repetitions)
			{
				tmp = *(alph + j);
				*(alph + j) = *(alph + j + 1);
				*(alph + j + 1) = tmp;
			}
		}
	}

}

void MakeBinCode(SymbolData* alph, int alph_size, int position)
{
	//printf("\n-----------------\n");
	if (alph_size == 1)
	{
		alph[0].bin_code[position] = '\0';
		return;
	}
	int i = 0;
	int beg = 0;
	int end = 0;
	for (; i < alph_size; i++)
	{
		int left = 0;
		int right = 0;
		for (int j = 0; j <= i; j++)
		{
			left += alph[j].repetitions;
		}
		for (int j = i + 1; j < alph_size; j++)
		{
			right += alph[j].repetitions;
		}
		beg = (left - right);
		end = (left + alph[i + 1].repetitions - (right - alph[i + 1].repetitions));
		if (abs(beg) <= abs(end)) break;
	}
	i++;
	for (int j = 0; j < i; j++)
	{
		alph[j].bin_code[position] = '0';
		//printf("%c -> %s\n", alph[j].symbol, alph[j].bin_code);
	}

	for (int j = i; j < alph_size; j++)
	{
		alph[j].bin_code[position] = '1';
		//printf("%c -> %s\n", alph[j].symbol, alph[j].bin_code);
	}
	MakeBinCode(&alph[0], i, ++position);
	//printf("\n---------m--------\n");
	MakeBinCode(&alph[i], alph_size - i, position);
	return;
}

void PrintData(SymbolData* alph, int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("\nSymbol               ='%c'\n", alph[i].symbol);
		printf("Count of repetitions = %d\n", alph[i].repetitions);
		printf("Binary code          = %s\n\n", alph[i].bin_code);
	}
}

void PrintBinToFile(char* file_out, int* bin_text, int len_bin_text, int* symbols_for_compression, int* not_fit)
{
	FILE* out = fopen(file_out, "a");
	if (out == NULL)FileError();
	*symbols_for_compression = len_bin_text / BYTE; //�������� ����� ���-�� ��������, ������� �� ����� �������� ��� ������
	
	int index = 0; //����� ��������� �� ������� bin_text_from_file

	char* s_f_c = (char*)calloc(*symbols_for_compression + 1, sizeof(char));
	if (s_f_c == NULL)
	{
		printf("no memory");
		exit(-1);
	}
	//������� �� ��������� ���� � ������� ������
	//printf("compression symbols:\n");
	for (int i = 0; i < *symbols_for_compression; i++)
	{
		for (int j = 0; j < BYTE; j++)
		{
			s_f_c[i] += (char)(bin_text[index]) * (char)pow(2, BYTE - 1 - j);
			index++;
		}
		/*if (s_f_c[i] == '\r')
		{
			fprintf(out, "\n");
			fprintf(out, "\\r");
			fprintf(out, "\n");
		}
		else if (s_f_c[i] == '\n')
		{
			fprintf(out, "\n");
			fprintf(out, "\\n");
			fprintf(out, "\n");
		}
		else if (s_f_c[i] == '\t')
		{
			fprintf(out, "\n");
			fprintf(out, "\\t");
			fprintf(out, "\n");
		}
		else
		{
			fprintf(out, "%c", s_f_c[i]);
		}*/
	}
	fwrite(s_f_c, sizeof(char), *symbols_for_compression, out);
	//printf("\ns_f_c - %s\n", s_f_c);
	//fprintf(out, "%s\n", s_f_c);
	//�������
	//printf("%s", s_f_c);
	
	free(s_f_c);

	fprintf(out, "\n");
	//������ ������� � ��������, ������� �� ������ � ������� ������
	*not_fit = len_bin_text - *symbols_for_compression * BYTE;
	for (int j = 0; j < *not_fit; j++)
	{
		fprintf(out, "%d", bin_text[index]);
		//�������
		//printf("%d", bin_text[index]);
		//
		index++;
	}
	

	fclose(out);
}

void DecToBin(unsigned char n, int* s, int* tmp, int step)
{
	step++;
	if (n >= 2)
	{
		DecToBin(n / 2, s, tmp, step);
	}
	int q = n % 2;
	int w = *tmp - step;
	s[w] = q;
}

int WhatFile(char* in)
{
	int i = 0;
	for (; (in[i] != '\0') && (i < MAX_FILE); i++);

	if (in[i - 3] == 'd' && in[i - 2] == 'o' && in[i - 1] == 'c')
		return BIN;
	else if (in[i - 3] == 't' && in[i - 2] == 'x'&& in[i - 1] == 't')
		return TEXT;
	else if (in[i - 3] == 'e' && in[i - 2] == 'x' && in[i - 1] == 'e')
		return EXE;
	else return ERROR;
}

int DocToText(char* new_name)
{
	FILE* fin = fopen(new_name, "rb");
	if (fin == NULL)FileError();
	//������� ���-�� �������� � ������
	fseek(fin, 0xa00, SEEK_SET);
	char c_f_in;
	int count = 0;
	while ((c_f_in = fgetc(fin)) && (c_f_in != NULL))
	{
		count++;
	}
	char* text_from_bin_file = (char*)calloc(count, sizeof(char));
	if (text_from_bin_file == NULL) printf("no memory!");
	//������ ��� ������� � ���������� � ������
	fseek(fin, 0xa00, SEEK_SET);
	fread(text_from_bin_file, sizeof(char), count, fin);
	text_from_bin_file[count-1] = '\0';
	fclose(fin);
	//printf("%s\n", text_from_bin_file);

	//�������� ����� ��� ������ ��������� �����
	int count_of_name = 0;
	while (new_name[count_of_name] != '\0')
	{
		++count_of_name;
	}
	new_name[count_of_name - 3] = 't';
	new_name[count_of_name - 2] = 'x';
	new_name[count_of_name - 1] = 't';
	//printf("%s\n", new_name);

	fin = fopen(new_name, "w");
	if (fin == NULL) FileError();
	fwrite(text_from_bin_file, sizeof(char), count, fin);
	fclose(fin);

	free(text_from_bin_file);
	return count;
}

int ExeToText(char* new_name)
{
	FILE* fin = fopen(new_name, "rb");
	if (fin == NULL)FileError();
	//������� ���-�� �������� � ������
	fseek(fin, 0x00, SEEK_END);
	char c_f_in;
	int count = ftell(fin);
//	printf("%d\n", count);
	rewind(fin);
	//while (!feof(fin)) //�� �������� ���� ����
	//{
	//	c_f_in = fgetc(fin);
	//	count++;
	//}
	char* text_from_bin_file = (char*)calloc(count, sizeof(char));
	if (text_from_bin_file == NULL) printf("no memory!");
	//������ ��� ������� � ���������� � ������
	fseek(fin, 0, SEEK_SET);
	int help_count = fread(text_from_bin_file, sizeof(char), count, fin);
//	printf("%d\n", help_count);
	text_from_bin_file[count - 1] = '\0';
	fclose(fin);
//	printf("%s\n", text_from_bin_file);

	//�������� ����� ��� ������ ��������� �����
	int count_of_name = 0;
	while (new_name[count_of_name] != '\0')
	{
		++count_of_name;
	}
	new_name[count_of_name - 3] = 't';
	new_name[count_of_name - 2] = 'x';
	new_name[count_of_name - 1] = 't';
//	printf("new_name = %s\n", new_name);

	fin = fopen(new_name, "w");
	if (fin == NULL)FileError();
	fwrite(text_from_bin_file, sizeof(char), count, fin);
	fclose(fin);

	free(text_from_bin_file);

	return count;
}

void FileError()
{
	perror("Error: file not open");
	exit(-1);
}

int TXTSize(char* new_name)
{
	FILE* fin = fopen(new_name, "r");
	if (fin == NULL)FileError();
	//������� ���-�� �������� � ������
	fseek(fin, 0x00, SEEK_END);
	char c_f_in;
	int count = ftell(fin);
//	printf("TextSize = %d\n", count);
	fclose(fin);
	return count;
}
