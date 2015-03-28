#include "FSM_read_module.h"
#define BUFFER_SIZE 500
static char buffer[BUFFER_SIZE];
static char *lexeme_begin , *forward;

void init() {
    fread(buffer, sizeof(char), BUFFER_SIZE, stdin);
    lexeme_begin = buffer;
    forward = buffer;
}

static void reset_states() {
    match_kywrd('a', 1);
    match_int('a', 1);
    match_float('a', 1);
    match_arith('a', 1);
    match_assgn('a', 1);
    match_relate('a', 1);
    catch_spcl_sym('a', 1);
    catch_comment('a', 1);
}

static void skip_white_spaces() {
    while(isspace(*forward)) forward++;
    lexeme_begin = forward;
}

void state_trans(char c, int *A) {
    int temp = -2, temp1, temp2;
    temp = match_kywrd(c,0);
    temp1 = temp;
    temp2 = 0;
    temp = match_int(c,0);
    if(temp>temp1) {
	temp1 = temp;
	temp2 = INT_CONST;
    }
    temp = match_float(c,0);
    if(temp>temp1) {
	temp1 = temp;
	temp2 = FLO_CONST;
    }
    temp = match_arith(c,0);
    if(temp>temp1) {
	temp1 = temp;
	temp2 = 1;
    }
    temp = match_assgn(c,0);
    if(temp>temp1) {
	temp1 = temp;
	temp2 = 2;
    }
    temp = match_relate(c,0);
    if(temp>temp1) {
	temp1 = temp;
	temp2 = 3;
    }
    temp = catch_spcl_sym(c,0);
    if(temp>temp1) {
	temp1 = temp;
	temp2 = 4;
    }
    temp = catch_comment(c,0);
    if(temp>temp1) {
	temp1 = temp;
	temp2 = -1;
    }
    A[0] = temp1;
    A[1] = temp2;
}

void str_cpy(char *dest, char *src, int length) {
    int i;
    for(i=0;i<length;i++) dest[i]=src[i];
    dest[length] = '\0';
    return;
}

void get_lex_tok(lexeme* l, HASH_T* symbol_table) {
    if(l->val==0) { //its a keyword
	int flag = get_keyword(symbol_table, l->lex);
	if(flag==-1) { //keyword not found in the symbol table
	    l->val = IDNTIFIER;
	    add_keyword(symbol_table,l->lex,1);
	}
	else if(flag==1) { //keyword found in the symbol table and is identified as an identifier
	    l->val = IDNTIFIER;
	}
	else if(flag==0) { //keyword found in the symbol table and is identified as a reserved keyword
	if(strcmp(l->lex,"short")==0) l->val=SHORT;
	if(strcmp(l->lex,"int")==0) l->val=INT;
	if(strcmp(l->lex,"float")==0) l->val=FLOAT;
	if(strcmp(l->lex,"double")==0) l->val=DOUBLE;
	if(strcmp(l->lex,"bool")==0) l->val=BOOL;
	if(strcmp(l->lex,"char")==0) l->val=CHAR;
	if(strcmp(l->lex,"signed")==0) l->val=SIGNED;
	if(strcmp(l->lex,"unsigned")==0) l->val=UNSIGNED;
	if(strcmp(l->lex,"for")==0) l->val=FOR;
	if(strcmp(l->lex,"while")==0) l->val=WHILE;
	if(strcmp(l->lex,"do")==0) l->val=DO;
	if(strcmp(l->lex,"return")==0) l->val=RETURN;
	if(strcmp(l->lex,"struct")==0) l->val=STRUCT;
	if(strcmp(l->lex,"const")==0) l->val=CONST;
	if(strcmp(l->lex,"void")==0) l->val=VOID;
	if(strcmp(l->lex,"switch")==0) l->val=SWITCH;
	if(strcmp(l->lex,"break")==0) l->val=BREAK;
	if(strcmp(l->lex,"case")==0) l->val=CASE;
	if(strcmp(l->lex,"continue")==0) l->val=CONTINUE;
	if(strcmp(l->lex,"goto")==0) l->val=GOTO;
	if(strcmp(l->lex,"long")==0) l->val=LONG;
	if(strcmp(l->lex,"static")==0) l->val=STATIC;
	if(strcmp(l->lex,"union")==0) l->val=UNION;
	if(strcmp(l->lex,"default")==0) l->val=DEFAULT;
	}
    }
    else if(l->val==1) { //its an arithmetic operator
	if(strcmp(l->lex,"+")==0) l->val=PLUS;
	if(strcmp(l->lex,"-")==0) l->val=MINUS;
	if(strcmp(l->lex,"*")==0) l->val=MULT;
	if(strcmp(l->lex,"/")==0) l->val=DIV;
	if(strcmp(l->lex,"%")==0) l->val=PERCENT;
	if(strcmp(l->lex,"++")==0) l->val=PLUS_PLUS;
	if(strcmp(l->lex,"--")==0) l->val=MINUS_MINUS;
    }
    else if(l->val==2) { //assignment operator
	if(strcmp(l->lex,"=")==0) l->val=EQ;
	if(strcmp(l->lex,"+=")==0) l->val=PLUS_EQ;
	if(strcmp(l->lex,"-=")==0) l->val=MINUS_EQ;
	if(strcmp(l->lex,"*=")==0) l->val=MULT_EQ;
	if(strcmp(l->lex,"/=")==0) l->val=DIV_EQ;
    }
    else if(l->val==3) { // relatiion operator
	if(strcmp(l->lex,"==")==0) l->val=EQ_EQ;
	if(strcmp(l->lex,"<")==0) l->val=LESSER;
	if(strcmp(l->lex,"<=")==0) l->val=LESS_EQ;
	if(strcmp(l->lex,">")==0) l->val=GREATER;
	if(strcmp(l->lex,">=")==0) l->val=GREAT_EQ;
    }
    else if(l->val==4) { //special symbol
	if(strcmp(l->lex,"[")==0) l->val=L_SQUARE;
	if(strcmp(l->lex,"]")==0) l->val=R_SQUARE;
	if(strcmp(l->lex,"(")==0) l->val=L_CBRACE;
	if(strcmp(l->lex,")")==0) l->val=R_CBRACE;
	if(strcmp(l->lex,"{")==0) l->val=L_PARAEN;
	if(strcmp(l->lex,"}")==0) l->val=R_PARAEN;
	if(strcmp(l->lex,",")==0) l->val=COMMA;
	if(strcmp(l->lex,";")==0) l->val=SEMICOL;
    }
}

lexeme *get_next_lexeme(HASH_T* symbol_table) {
    int lexeme_length = 0, flag = 1, temp[2];
    reset_states();
    skip_white_spaces();
    if(*forward=='\0'||*forward=='\177'||*forward=='\377') return NULL;
    while(flag) {
	state_trans(*forward, temp);

	if(temp[0]<=lexeme_length) flag = 0;
	else {
	    forward++;
	    lexeme_length = temp[0];
	}
    }
    lexeme* new_l = (lexeme*)malloc(sizeof(lexeme));
    new_l->lex = NULL;
    new_l->val = temp[1];
    if(temp[1] == -1) return new_l;
    new_l->lex = (char*)malloc((lexeme_length+1)*sizeof(char));
    str_cpy(new_l->lex, lexeme_begin, lexeme_length);
    get_lex_tok(new_l,symbol_table);
    return new_l;
}

void free_lexeme(lexeme* l) {
    free(l->lex);
    free(l);
}

