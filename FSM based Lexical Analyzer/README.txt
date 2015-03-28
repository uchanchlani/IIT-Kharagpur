Assignment 4
Part a
Designing a lexical analyser by Automatons

In this folder we have 5 c files and 4 header files
FSM_read_module.h and .c is the reading module which reads the input stream (from the standard input) and generates the tokens from that input stream.
symtab.h and .c is the file which generates the symbol table and does all the necessary things of the symtable.
FSM_write_module.h and .c is the writing module, which takes the tokens and simply writes it in the output file.
FSM_state.h and .c is the file which has all the automatons to identify the tokens.
MAIN.c is the main c file.

2 .in files:
input.in is the input file which contains the string to be parsed and is parsed through the stdin\
sym_tab.in is the symbol table file and is used by the symtab file via FILE handling

2 .out files:
a4_1.out is the output stream of tokens
sym_tab_1.out is the output symbol table from the parser

1 object file namely "FSM"

And 1 .h file which was provided to us "def.h"

compile as:
gcc MAIN.c FSM_read_module FSM_write_module FSM_state.c symtab.c -o FSM

Run the program as:
./FSM < input.in

Linking
FSM_read_module uses the FSM_state and the symtab files
symtab and FSM_state are the basic libraries which doesn't use any custom libraries that we've made
FSM_write_module uses the FSM_read_module only for the "lexeme struct" defined in the read_module
MAIN uses the symtab, FSM_read_module and FSM_write_module...


Submitted by:
Utkarsh Chanchlani
11CS10049
Rupesh Kumar
11CS30028

