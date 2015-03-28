/*
 * LL(1) parser
 * Group 29
 */

#include<stdio.h>
#include "grammer_table_module.h"
#include "parser_module.h"

#define MAX_SYMBOLS 100
//#define debug

extern FILE *yyout;

int yywrap() {
    printf("TOKENS GENERATED!!\n");
    return 1;
}

int main() {
    // the lexical analysis part
    FILE *fp;
    fp = fopen("tokens.txt","w");
    yyout = fp;
    yylex();
    fclose(fp);

//    int i,j;

    // initiallizing the grammer struct, first-follow struct and the parse table struct (all functions in the grammer & table module)
    GRAMMER my_grammer = NULL;
    FstFlw my_first_follow = NULL;
    P_Table my_parse_table = create_parse_table(MAX_SYMBOLS);

    // loading the hashed grammer, hashed first-follow, their meaning and filling the parsing table (all functions in the grammer & table module)
    char** meaning = load_meaning_set("meaning.txt");
    load_grammer("grammer.txt", &my_grammer);
    load_FirstFollow("fst-flw.txt",&my_first_follow);
    fill_parse_table(my_parse_table,my_grammer,my_first_follow);

    // scanning the hgenerated tokens to the input string
    fp = fopen("tokens.txt","r");
    char* input = malloc(50*sizeof(char));
    fscanf(fp,"%s",input);
    fclose(fp);

    // parse string function (in the parser module
    parse_string(input, my_parse_table, my_grammer, meaning);

#ifdef debug
    //printing the parse table
    for(i = 0; i< 30;i++){
	int count = 0;
	for(j=0;j<30;j++){
	    printf("%d  ",my_parse_table[i][j]);
	    if(my_parse_table[i][j]>0)
		count++;
	}
	printf("count = %d\n",count);
    }
#endif

    // free all the dynamic memories
    free_parse_table(&my_parse_table, MAX_SYMBOLS);
    free_grammer(&my_grammer);
    free_firstFollow(&my_first_follow);
    free_meaning_set(&meaning);

    return 0;
}

