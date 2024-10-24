%{
#include <stdio.h>
int error_flag = 0;
void yyerror(const char *s);
extern int yylex(void);
extern int yylineno;
%}


/* %union{
	int		number;
	char		strValue[50];
}; */

%token ID INT
%token ADD MUL SUB DIV MOD ASSIGN NOT LE GE LT GT EQ NE AND OR LBRACE RBRACE LPAREN RPAREN SEMICOLON LBRACKET RBRACKET COMMA
%token AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO IF INTTYPE LONG REGISTER RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID VOLATILE WHILE
%%

CompUnit      : CompUnit Decl | CompUnit FuncDef | Decl | FuncDef;

Decl          : VarDecl;
VarDecl       : INTTYPE VarDefList SEMICOLON;

VarDefList    : VarDef
                | VarDefList COMMA VarDef
                ;

VarDef        : ID BRACKETList
                | ID BRACKETList ASSIGN InitVal
                ;
BRACKETList     : 
                | BRACKETList LBRACKET INT RBRACKET


InitVal       : Exp
                | LBRACE OptInitValList RBRACE
                ;
OptInitValList: /* empty */
                | InitValList
                ;
InitValList   : InitVal
                | InitValList COMMA InitVal
                ;

FuncDef: INTTYPE ID LPAREN FuncFParamsOpt RPAREN LBRACE BlockItems RBRACE
        | VOID ID LPAREN FuncFParamsOpt RPAREN LBRACE BlockItems RBRACE
        ;


FuncFParamsOpt: /* empty */
              | FuncFParams
              ;

FuncFParams: FuncFParam
           | FuncFParams COMMA FuncFParam
           ;

FuncFParam: INTTYPE ID ArrayOpt
          ;

ArrayOpt: /* empty */
        | LBRACKET RBRACKET BRACKETList
        ;


BlockItems: /* empty */
          | BlockItems BlockItem
          ;

BlockItem: Decl
         | Stmt
         ;

Stmt: MatchedStmt | OpenStmt ;

MatchedStmt:
      IF LPAREN Cond RPAREN MatchedStmt ELSE MatchedStmt
    | NonIfStmt
    ;

OpenStmt:
      IF LPAREN Cond RPAREN Stmt
    | IF LPAREN Cond RPAREN MatchedStmt ELSE OpenStmt
    ;

NonIfStmt:
      LVal ASSIGN Exp SEMICOLON
    | OptExp SEMICOLON
    | LBRACE BlockItems RBRACE
    | WHILE LPAREN Cond RPAREN Stmt
    | RETURN OptExp SEMICOLON
    ;

OptExp: /* empty */
      | Exp
      ;

Exp:  AddExp;

Cond: LOrExp;

LVal: ID Arrays;

PrimaryExp: LPAREN Exp RPAREN
          | LVal
          | Number
          ;

Number: INT;

UnaryExp: PrimaryExp
        | ID LPAREN FuncRParamsOpt RPAREN
        | UnaryOp UnaryExp
        ;

UnaryOp: ADD
       | SUB
       | NOT
       ;

FuncRParamsOpt: /* empty */
              | FuncRParams
              ;

FuncRParams: Exp
           | FuncRParams COMMA Exp
           ;

Arrays: /* empty */
        | LBRACKET Exp RBRACKET Arrays
        ;

MulExp: UnaryExp
      | MulExp MUL UnaryExp
      | MulExp DIV UnaryExp
      | MulExp MOD UnaryExp
      ;

AddExp: MulExp
      | AddExp ADD MulExp
      | AddExp SUB MulExp
      ;

RelExp: AddExp
      | RelExp LT AddExp
      | RelExp GT AddExp
      | RelExp LE AddExp
      | RelExp GE AddExp
      ;

EqExp: RelExp
     | EqExp EQ RelExp
     | EqExp NE RelExp
     ;

LAndExp: EqExp
        | LAndExp AND EqExp
        ;

LOrExp: LAndExp
       | LOrExp OR LAndExp
       ;
%%

void yyerror(const char *s) {
    printf("Error: %s, At line %d \n", s, yylineno);
    
    error_flag = 1;
}
