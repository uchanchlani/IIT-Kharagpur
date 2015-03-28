#pragma once
#include<stdio.h>
#define RUNNING 100
#define ACCEPT 101

int match_kywrd(char c, int reset);
int match_int(char c, int reset);
int match_float(char c, int reset);
int match_arith(char c, int reset);
int match_assgn(char c, int reset);
int match_relate(char c, int reset);
int catch_spcl_sym(char c, int reset);
int catch_comment(char c, int reset);
//void print_error(FILE *fp);
//void print_lxm(char *buffer, int bfr_index, int state, int st_index, FILE *fp_a4);

