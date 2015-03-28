#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "grammer_table_module.h"

// the struct for the stack
typedef struct _node {
    char val;
    struct _node *next;
} node, *STACK;

// parses the string S using the parse table
void parse_string(char *S, P_Table P, GRAMMER G, char** meaning);

