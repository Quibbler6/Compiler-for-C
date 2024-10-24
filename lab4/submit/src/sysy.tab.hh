/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_SYSY_TAB_HH_INCLUDED
# define YY_YY_SRC_SYSY_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 1 "src/sysy.y"

  #include <memory>
  #include <string>

#line 54 "src/sysy.tab.hh"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    INT = 259,                     /* INT  */
    ADD = 260,                     /* ADD  */
    MUL = 261,                     /* MUL  */
    SUB = 262,                     /* SUB  */
    DIV = 263,                     /* DIV  */
    MOD = 264,                     /* MOD  */
    ASSIGN = 265,                  /* ASSIGN  */
    NOT = 266,                     /* NOT  */
    LE = 267,                      /* LE  */
    GE = 268,                      /* GE  */
    LT = 269,                      /* LT  */
    GT = 270,                      /* GT  */
    EQ = 271,                      /* EQ  */
    NE = 272,                      /* NE  */
    AND = 273,                     /* AND  */
    OR = 274,                      /* OR  */
    LBRACE = 275,                  /* LBRACE  */
    RBRACE = 276,                  /* RBRACE  */
    LPAREN = 277,                  /* LPAREN  */
    RPAREN = 278,                  /* RPAREN  */
    SEMICOLON = 279,               /* SEMICOLON  */
    LBRACKET = 280,                /* LBRACKET  */
    RBRACKET = 281,                /* RBRACKET  */
    COMMA = 282,                   /* COMMA  */
    AUTO = 283,                    /* AUTO  */
    BREAK = 284,                   /* BREAK  */
    CASE = 285,                    /* CASE  */
    CHAR = 286,                    /* CHAR  */
    CONST = 287,                   /* CONST  */
    CONTINUE = 288,                /* CONTINUE  */
    DEFAULT = 289,                 /* DEFAULT  */
    DO = 290,                      /* DO  */
    DOUBLE = 291,                  /* DOUBLE  */
    ELSE = 292,                    /* ELSE  */
    ENUM = 293,                    /* ENUM  */
    EXTERN = 294,                  /* EXTERN  */
    FLOAT = 295,                   /* FLOAT  */
    FOR = 296,                     /* FOR  */
    GOTO = 297,                    /* GOTO  */
    IF = 298,                      /* IF  */
    INTTYPE = 299,                 /* INTTYPE  */
    LONG = 300,                    /* LONG  */
    REGISTER = 301,                /* REGISTER  */
    RETURN = 302,                  /* RETURN  */
    SHORT = 303,                   /* SHORT  */
    SIGNED = 304,                  /* SIGNED  */
    SIZEOF = 305,                  /* SIZEOF  */
    STATIC = 306,                  /* STATIC  */
    STRUCT = 307,                  /* STRUCT  */
    SWITCH = 308,                  /* SWITCH  */
    TYPEDEF = 309,                 /* TYPEDEF  */
    UNION = 310,                   /* UNION  */
    UNSIGNED = 311,                /* UNSIGNED  */
    VOID = 312,                    /* VOID  */
    VOLATILE = 313,                /* VOLATILE  */
    WHILE = 314                    /* WHILE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 226 "src/sysy.y"

	int int_val;
      std::string *str_val;
      BaseAST *ast_val;
      struct InitList {
            struct InitList* next;  // 指向下一个节点，值或子列表
            int isValue;            // 是否为值，0代表子列表，1代表值
            union {
                  int value;          // 值节点的值
                  struct InitList* child; // 子列表节点
            } data;
      } * initlist;

#line 144 "src/sysy.tab.hh"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (std::unique_ptr<BaseAST> &AST);


#endif /* !YY_YY_SRC_SYSY_TAB_HH_INCLUDED  */
