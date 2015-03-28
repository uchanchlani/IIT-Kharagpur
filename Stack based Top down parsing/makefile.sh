#!/bin/bash

echo "compiling";
flex G29.7.l
gcc G29_7.c lex.yy.c grammer_table_module.c parser_module.c -o llparser
echo "object file created is llparser";
