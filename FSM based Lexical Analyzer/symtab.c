#include "symtab.h"



/* This function loads the keywords and their codes(ie 1) provided in the symbol table */
void load_symtab(char *S, HASH_T *H) {
    // H contains the address of the table storing the operators and their opcodes
    if((*H)!=NULL) {printf("\nLIST not empty. Pass an empty list!!"); return;}
    HASH_T p = NULL;
    int i;
	
    //used to take opcode as a string and then convert it to int
    char buffer[5];
    FILE *fp;
    fp = fopen(S, "r"); //S is the string name entered
    if(fp==NULL) {printf("\nFile doesn't exists!"); return;}
	
    while( !feof(fp) ) {
	HASH_T temp = (HASH_T)malloc(sizeof(hash));
	temp->next = NULL;
	fscanf(fp, "%s %s", (temp->keyword), buffer);
	temp->val = strtol(buffer,NULL,10);
	if(p==NULL && strcmp(temp->keyword,"")) {p = temp; *H = temp;}
	else if(strcmp(temp->keyword,"")) {p->next = temp; p = p->next;}
	else {free(temp);}
    }
    fclose(fp);
    return;
	
}

/*writes the symbol table back to the file*/
void write_symtab(char *S, HASH_T *H) {
    HASH_T p = *H;
    FILE *fp;
    fp = fopen(S, "w");
    if(fp==NULL) {printf("\nFile doesn't exists!"); return;}
    while(p!=NULL) {
	fprintf(fp, "%s %d\n", p->keyword, p->val);
	p = p->next;
    }
    fclose(fp);
    return;
}

/*returns the value of the keyword and if not found returns error (-1)*/
int get_keyword(HASH_T *H, char *key) {
    if(*H == NULL) return -1;
    HASH_T p = *H;
    while(p!=NULL) {
	if(strcmp(p->keyword, key) == 0) return p->val;
	p = p->next;
    }
    return -1;
}

/* adds the keyword with given value*/
void add_keyword(HASH_T *H, char *key, int val) {
    if(*H == NULL) {
	*H = (HASH_T)malloc(sizeof(hash));
	strcpy((*H)->keyword,key);
	(*H)->val = val;
	return;
    }
    HASH_T p = *H;
    while(p->next!=NULL) p=p->next;
    p->next = (HASH_T)malloc(sizeof(hash));
    strcpy(p->next->keyword,key);
    p->next->val = val;
    return;
}


