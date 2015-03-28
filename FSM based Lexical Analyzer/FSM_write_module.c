#include "FSM_write_module.h"

FILE* write_lexeme_to_file(lexeme* l, FILE *fp) {
//	static FILE *fp = fopen("a4_1.out", "w");
	if(l->val==300||l->val==301||l->val==302) fprintf(fp, "%d %s\n", l->val, l->lex);
	else fprintf(fp, "%d\n", l->val, l->lex);
	return fp;
}

