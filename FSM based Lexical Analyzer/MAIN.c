/*
 * Assignment 4, part a
 * Finite State Machine
 */

#include<stdio.h>
#include "FSM_state.h"
#include "symtab.h"
#include "FSM_read_module.h"
#include "FSM_write_module.h"
#define NEG_MAX -100

int main() {
    printf("Initializing the lexical analyzer...\n\n");
    init();
    printf("Initializing the symbol table from sym_tab.txt file...\n\n");
    HASH_T symbol_table = NULL;
    load_symtab("sym_tab.in", &symbol_table);
    lexeme *my_lex = NULL;
    printf("Open file a4_1.out for writing the output...\n\n");
    FILE *fp = fopen("a4_1.out", "w");
    while(1) {
	my_lex = get_next_lexeme(&symbol_table);
	if(my_lex == NULL) break;
	fp = write_lexeme_to_file(my_lex, fp);
	free_lexeme(my_lex);
    }

    printf("Write symbol table to sym_tab_1.out file...\n\n");
    write_symtab("sym_tab_1.out", &symbol_table);
    printf("WINDING UP...\n\n");
    fclose(fp);
    return 0;
}

