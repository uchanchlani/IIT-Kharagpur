#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


/*This takes the opcodes of the operators to generate the object code*/
typedef struct _hash {
	char keyword[20];
	int val;
	struct _hash *next;
} hash, *HASH_T;

void load_symtab(char *S, HASH_T *H);
int get_keyword(HASH_T *H, char *key);
void add_keyword(HASH_T *H, char *key, int val);
void write_symtab(char *S, HASH_T *H);
