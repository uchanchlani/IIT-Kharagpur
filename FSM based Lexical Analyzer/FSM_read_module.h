#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "def.h"
#include "FSM_state.h"
#include "symtab.h"

typedef struct {
	char *lex;
	int val;
} lexeme;

void init();
lexeme *get_next_lexeme(HASH_T *symbol_table);
void free_lexeme(lexeme*);

