%option noyywrap
%option yylineno
%option noinput
%option nounput
%option noyywrap

%{

#include <cstdlib>
#include <string>
#include "ast.h"

// 因为 Flex 会用到 Bison 中关于 token 的定义
// 所以需要 include Bison 生成的头文件
#include "sysy.tab.hh"
extern int error_flag;
extern char * yystr;
using namespace std;

%}


digit [0-9]
identifier_nondigit [_a-zA-Z]
hex_digit [0-9A-Fa-f]
nonzero_digit [1-9]
oct_digit [0-7]
blank [ \t\n]

%x COMMENT

%%
"/*"                   { BEGIN(COMMENT); }
<COMMENT>"*/"          { BEGIN(INITIAL); /* 正常结束注释 */ }
<COMMENT>.|\n          { /* 吸收注释内的字符 */ }


[_a-zA-Z][_a-zA-Z0-9]*    {
    if(strcmp(yytext, "auto") == 0)
        return AUTO;
    else if(strcmp(yytext, "break") == 0)
        return BREAK;
    else if(strcmp(yytext, "case") == 0)
        return CASE;
    else if(strcmp(yytext, "char") == 0)
        return CHAR;
    else if(strcmp(yytext, "const") == 0)
        return CONST;
    else if(strcmp(yytext, "continue") == 0)
        return CONTINUE;
    else if(strcmp(yytext, "default") == 0)
        return DEFAULT;
    else if(strcmp(yytext, "do") == 0)
        return DO;
    else if(strcmp(yytext, "double") == 0)
        return DOUBLE;
    else if(strcmp(yytext, "else") == 0)
        return ELSE;
    else if(strcmp(yytext, "enum") == 0)
        return ENUM;
    else if(strcmp(yytext, "extern") == 0)
        return EXTERN;
    else if(strcmp(yytext, "float") == 0)
        return FLOAT;
    else if(strcmp(yytext, "for") == 0)
        return FOR;
    else if(strcmp(yytext, "goto") == 0)
        return GOTO;
    else if(strcmp(yytext, "if") == 0)
        return IF;
    else if(strcmp(yytext, "int") == 0)
        return INTTYPE;
    else if(strcmp(yytext, "long") == 0)
        return LONG;
    else if(strcmp(yytext, "register") == 0)
        return REGISTER;
    else if(strcmp(yytext, "return") == 0)
        return RETURN;
    else if(strcmp(yytext, "short") == 0)
        return SHORT;
    else if(strcmp(yytext, "signed") == 0)
        return SIGNED;
    else if(strcmp(yytext, "sizeof") == 0)
        return SIZEOF;
    else if(strcmp(yytext, "static") == 0)
        return STATIC;
    else if(strcmp(yytext, "struct") == 0)
        return STRUCT;
    else if(strcmp(yytext, "switch") == 0)
        return SWITCH;
    else if(strcmp(yytext, "typedef") == 0)
        return TYPEDEF;
    else if(strcmp(yytext, "union") == 0)
        return UNION;
    else if(strcmp(yytext, "unsigned") == 0)
        return UNSIGNED;
    else if(strcmp(yytext, "void") == 0)
        return VOID;
    else if(strcmp(yytext, "volatile") == 0)
        return VOLATILE;
    else if(strcmp(yytext, "while") == 0)
        return WHILE;
    else{
        //printf("ID(%s)\n",yytext);
        yylval.str_val = new string(yytext);
        return ID;
    }
}
"//"(.)*  {}

[1-9][0-9]*  {yylval.int_val = atoi(yytext);  return INT;}
("0x"|"0X"){hex_digit}+ {yylval.int_val = strtol(yytext, NULL, 16);  return INT;}
"0"{oct_digit}+         {yylval.int_val = strtol(yytext, NULL, 8);  return INT;}
"0" {yylval.int_val = atoi(yytext);  return INT;}
"+"     { return ADD; }
"-"     { return SUB; }
"*"     { return MUL; }
"/"     { return DIV; }
"%"     { return MOD; }
"="     { return ASSIGN; }
"!"     { return NOT; }
"<="     { return LE; }
">="     { return GE; }
"<"     { return LT; }
">"     { return GT; }
"=="     { return EQ; }
"!="     { return NE; }
"&&"     { return AND; }
"||"     { return OR; }
"{"     { return LBRACE; }
"}"     { return RBRACE; }
"("     { return LPAREN; }
")"     { return RPAREN; }
";"     { return SEMICOLON; }
"["     { return LBRACKET; }
"]"     { return RBRACKET; }
","     { return COMMA; }
{blank}   {}
.       { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); error_flag = 1;}
%%

/* int main(int argc, char** argv) {
    if(argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            perror(argv[1]);
            return 1;
        }
    }
    yylex(); // 调用词法分析器
    return 0;
} */

