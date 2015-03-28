%{

int count = 1, check = 0;

%}

digit          [0-9]
id             -?{digit}+
eol            \n
space          [ \t]
ws             {space}*
errid          [-]{space}+{digit}+

%%
{eol}		fprintf(yyout,"z\n");
{ws}		;
"+" 		fprintf(yyout,"a");
"*" 		fprintf(yyout,"b");
"#" 		fprintf(yyout,"c");
"." 		fprintf(yyout,"d");
"NORM" 		fprintf(yyout,"e");
{id} 		fprintf(yyout,"f");
{errid}         {fprintf(yyout,"f"); printf("LEXICAL ERROR... Space between the - and the digits...\n");}
"x" 		fprintf(yyout,"g");
"(" 		fprintf(yyout,"h");
")" 		fprintf(yyout,"i");
"[" 		{fprintf(yyout,"j"); count = 0;}
"]" 		{fprintf(yyout,"k"); if(check==0) check = count; else if(check!=count) printf("LEXICAL ERROR... Different number of arguements in the vectors...\n");}
"," 		{fprintf(yyout,"l"); count++;}
