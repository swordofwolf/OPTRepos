%option nounistd

%option noyywrap nodefault yylineno
%{
#include <ctype.h>
#include "codegen.h"
#include "parser.tab.h"
#include "symtable.h"

%}

char 		[^"\n]

%%
 

[ \t\n] ;   

">"  | 
"<"  | 
"!=" | 
"==" | 
">=" | 
"<="    { strcpy(yylval.string, yytext); return CMP; }

"&&" | "||" | "^^" {strcpy(yylval.string, yytext); return BOOLOP;}

":" |
"." |
"+" |
"-" |
"*" | 
"/" |
"=" | 
";" | 
"(" | 
")" | 
"{" | 
"}" |
"[" |
"]" |
"|" | 
"&" | 
"!" { return yytext[0];}

"goto" {return GOTO;}
"continue" { return CONTINUE; }
"break"		{ return BREAK; }
"if"  		{ return IF; }
"else"  	{ return ELSE; }
"while" 	{ return WHILE; }
"do"    	{ return DO; }
"for"   	{ return FOR; }
"struct"	{ return STRUCT; }
"union"		{ return UNION; }
"func"    { return FUNC; }


[a-zA-Z][a-zA-Z0-9]* { strcpy(yylval.string, yytext);
                       return VARIABLE;
                     }
0|[1-9][0-9]*        { yylval.i = atoi(yytext); return INTCONST; }
0[xX][0-9]*        { yylval.i = strtol(yytext, NULL, 16); return INTCONST; }
0[hH][0-9]*        { yylval.i = strtol(yytext, NULL, 8); return INTCONST; }
[\-]?[0-9]*(\.[0-9]+)? { yylval.f = (float) atof(yytext); return FLOATCONST;}
\"{char}*$	{ yyerror("unfinished string literal."); }
\"{char}*\"	{
			int len = strlen(yytext);
			strncpy(yylval.string, yytext+1, strlen(yytext) - 1);
			yylval.string[len - 2] = '\0';
			return(STRCONST);
			;
		}

.	{ yyerror("Not in alphabet %c\n", yytext[0]); }
%%
