#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define num_ops 23
char* define_str = "define";
char* if_str = "if";
char* quote_str = "quote";
char* car_str = "car";
char* cdr_str = "cdr";
char* cons_str = "cons";
char* atom_str = "atom";
char* print_str = "print";
char* progn_str = "progn";
char* while_str = "while";
char* lambda_str = "lambda";
char* macro_str = "macro";
char* closure_str = "closure";
char* eval_str = "eval";
char* eq_str = "eq";
char* plus_str = "+";
char* minus_str = "-";
char* ast_str = "*";
char* slash_str = "/";
char* mod_str = "mod";
char* lt_str = "<";
char* gt_str = ">";
char* t_str = "t";

//====================================

char* _str;
int q, r;
int i;
int j;
int k;

//====================================


int malloc_bytes;
void* malloc_k() {
    return malloc(malloc_bytes);
}
