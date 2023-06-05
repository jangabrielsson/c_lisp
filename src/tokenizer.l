/*
 * lisp.lex: A simple Lisp scanner
 *
 */

%option yylineno
%option noyywrap

%{
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "y.tab.h"

    int badtoken_cnt = 0;
    int token_cnt = 0;
    int col_cnt = 0;
    int lineno = 0;

    char lexBuffer[8192];
    static unsigned long lexBuffPtr = 0;

#define CLEARBUFF() (lexBuffer[lexBuffPtr=0]=0)
#define APPENDCHARTOBUFF(CH) {lexBuffer[lexBuffPtr++]=CH; lexBuffer[lexBuffPtr]=0;}
#define COPYSTRINGTOBUFF(STR) {strcpy(lexBuffer,STR); lexBuffPtr=strlen(STR);}
#define APPENDSTRINGTOBUFF(STR) { for(int i = 0; STR[i];) lexBuffer[lexBuffPtr++] = STR[i++]; lexBuffer[lexBuffPtr]=0;}

#undef YY_INPUT
#define YY_INPUT(b,r,s) readInputForLexer(b,&r,s)

%}

%x string
%x symbol

specquoted  (\\,|\\\.|\\`|\\'|\\&|\\#)
comment     \/\*([^*]|\n)*\*\/
digit       [0-9]
ichar       [A-Z_a-z_*@]
integer     [+-]?{digit}+
newline     \n
strchar     ([ ~]|\\n)
identifier  ({ichar}|{specquoted})+([0-9_-]|{ichar}|{specquoted})*
keyword     [&]{ichar}([0-9]|{ichar})*
message     {ichar}([0-9]|{ichar})*[:]
whitespace  [ \t]+
float       ([+-]?{digit}+)?\.{digit}*(e?[+-]?{digit}+)?
chrliteral  '([!*]|\\n)'
nullstring  \"\"
escquote    [^"]*\\\"[^"]*
strliteral  \"[^"]*{escquote}*\"
str2literal \"(\\.|[^\\"])*\"

%%

"#"             { COPYSTRINGTOBUFF(yytext); return tFUNCTION;}
"`"             { COPYSTRINGTOBUFF(yytext); return tBACK_QUOTE;}
"'"             { COPYSTRINGTOBUFF(yytext); return tQUOTE;}
"("             { COPYSTRINGTOBUFF(yytext); return tLBRACE;}
")"             { COPYSTRINGTOBUFF(yytext); return tRBRACE;}
"["             { COPYSTRINGTOBUFF(yytext); return tLBRACK;}
"]"             { COPYSTRINGTOBUFF(yytext); return tRBRACK;}
"."             { COPYSTRINGTOBUFF(yytext); return tDOT;}
">"             { COPYSTRINGTOBUFF(yytext); return tGREATER;}
"<"             { COPYSTRINGTOBUFF(yytext); return tLESS;}
"="             { COPYSTRINGTOBUFF(yytext); return tEQUAL;}
"+"             { COPYSTRINGTOBUFF(yytext); return tADD;}
"-"             { COPYSTRINGTOBUFF(yytext); return tSUB;}
"*"             { COPYSTRINGTOBUFF(yytext); return tMUL;}
"/"             { COPYSTRINGTOBUFF(yytext); return tDIV;}
","             { COPYSTRINGTOBUFF(yytext); return tCOMMA;}
",."            { COPYSTRINGTOBUFF(yytext); return tCOM_DOT;}
",@"            { COPYSTRINGTOBUFF(yytext); return tCOM_AT;}

{float}         { COPYSTRINGTOBUFF(yytext); return tFLOAT; }
{integer}       { COPYSTRINGTOBUFF(yytext); return tINTEGER; }
{chrliteral}    { COPYSTRINGTOBUFF(yytext); return tCHRLIT; }

{identifier}    { COPYSTRINGTOBUFF(yytext); return tATOM; }

{keyword}       { COPYSTRINGTOBUFF(yytext); return tKEYWORD; }
{message}       { COPYSTRINGTOBUFF(yytext); return tMESSAGE; }
{newline}       { col_cnt = 1; }
{whitespace}    { col_cnt+=yyleng; }
{comment}       { col_cnt = 0; }

\"                  { BEGIN string; CLEARBUFF();}
<string>[^\\"\n]*   { APPENDSTRINGTOBUFF(yytext); }
<string>\\n         { APPENDCHARTOBUFF('\n'); }
<string>\\\(         { APPENDCHARTOBUFF('('); }
<string>\\t         { APPENDCHARTOBUFF('\t'); }
<string>\\[0-7]*    { APPENDCHARTOBUFF((char)strtol(yytext+1, 0, 8)); }
<string>\\[\\"]     { APPENDCHARTOBUFF(yytext[1]); }
<string>\"          { BEGIN 0; return tSTRLIT; }
<string>\\.         { printf("bogus escape '%s' in string\n", yytext); return tERROR; }
<string>\n          { APPENDCHARTOBUFF('\n'); }

"//"            { /* handle C++ style comments */
    char c;
    do { c = input();
    } while (c != '\n');
    lineno++;
}

";"            { /* handle ;;; style comments */
    char c;
    do { c = input();
    } while (c != '\n');
    lineno++;
}

.               { printf("Unrecognized token '%s'\n", yytext); return tERROR;}

%%
