#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
//#include <ctype.h>

void MemoryError();

void FileErrorLZ();

struct WORD* Init_word0(struct WORD* dictionary);

void LZ78Encod(char* file_in, char* file_out);

void WriteToFile(char* file_out, struct WORD* Dictionary, int size_of_dictionary);

int find(char* tmp, struct WORD* Dictionary, int size_of_dictionary);

void FreeWords(struct WORD* Dictionary, int size_of_dictionary);

void LZ78Decod(char* file_out, char* file_res);

void SearchForWord(struct WORD* Dictionary, int ind, int size_of_dictionary, char* encode_str, int i);

void WriteAnswer(struct WORD* Dictionary, int size_of_dictionary, char* file_res);

//int SymbolToDictionary(char buf, WORD* dictionary, int* size_of_dictionary, char* work_str, int* index_work_str);