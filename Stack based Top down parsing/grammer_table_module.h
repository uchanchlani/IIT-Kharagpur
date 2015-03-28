#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// the blank entry in the table is shown by -1, and the synch entries by -2
#define SYNCH -2

// the struct to store the grammer
typedef struct _grm {
    int id;
    char head;
    char* body;
    struct _grm* next;
} grm, *GRAMMER;

// the struct to store the first and follow of the N-T's
typedef struct _ff {
    char head;
    char* first;
    char* follow;
    struct _ff* next;
} fflw, *FstFlw;

// parse table is basically an 2D integer array
typedef int** P_Table;

void load_grammer(char *S, GRAMMER *G); // loads the grammer from filename S
char get_head(GRAMMER G, int index); // returns the head of the production whose id is provided
char* get_body(GRAMMER G, int index); // returns the pointer to the head of the production whose id is provided
void print_grammer(GRAMMER G); // prints the grammer, (for debugging)
void free_grammer(GRAMMER *G); // frees the grammer struct

void load_FirstFollow(char *S, FstFlw *FF); // loads the first-follow set from filename S
char* get_first(FstFlw FF, char key); // returns the pointer to the first of the N-T "key"
char* get_follow(FstFlw FF, char key); // returns the pointer to the follow of the N-T "key"
void free_firstFollow(FstFlw *FF); // frees the struct

P_Table create_parse_table(int size); // creates a blank parse table (all entries are -1)
void fill_parse_table(P_Table P, GRAMMER G, FstFlw F); // fills the parsing table according to the grammer and first follow set
void free_parse_table(P_Table *P, int size); // frees the struct

/*
 * meaning set stores the actual meaning of the hashed value in it's respective position
 */
char** load_meaning_set(char* S); // loads the meaning set from filename S
char* get_meaning(char** M, char key); // returns the meaning of the hashed value "key"
void free_meaning_set(char*** M); //frees the meaning set

