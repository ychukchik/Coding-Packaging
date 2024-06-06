#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef unsigned long long ULL;


void FanoEncod(char* file_in, char* file_out);

int StructStrstr(char c, struct SymbolData* alph, int alph_size);

void Sorting(struct SymbolData* alph, int n);

void MakeBinCode(struct SymbolData* alph, int alph_size, int step);

void PrintData(struct SymbolData* alph, int n);

void PrintBinToFile(char* file_out, int* bin_text, int len_bin_text, int* symbols_for_compression, int* not_fit);

void FanoDecod(char* file_out, char* file_res);

void DecToBin(unsigned char n, int* s, int* tmp, int step);

int WhatFile(char* in);

int DocToText(char* file_in);

int ExeToText(char* new_name);

void FileError();

int TXTSize(new_name);

void OneSymbolEncod(char* file_out, struct SymbolData* alph);

void OneSymbolDecod(char* file_out, char* file_res);