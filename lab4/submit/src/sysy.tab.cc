/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 6 "src/sysy.y"


#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "sym_tlb.h"
#include "ast.h"
#include "IR_RISC_V.h"


// 声明 lexer 函数和错误处理函数
int yylex();
extern FILE *yyin;
extern FILE *yyout;
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);
int error_flag = 0;
extern int yylineno;
using namespace std;

#define SIZE 109
shared_ptr<bucket> table_parse[SIZE];
shared_ptr<bucket> table_dump[SIZE];

meta init_meta(InfoType type, std::vector<int> var, std::vector<Param> fun, int value) {
    meta m = new info;
    m->type = type;
    m->var = var;
    m->fun = fun;
    m->value = value;
    m->global = 0;
    return m;
}

void print_meta(meta m) {
    std::cout << "InfoType: ";
    switch (m->type) {
        case FUNCTION_INT:
            std::cout << "FUNCTION_INT";
            break;
        case FUNCTION_VOID:
            std::cout << "FUNCTION_VOID";
            break;
        case VARIABLE:
            std::cout << "VARIABLE";
            break;
        case NUMBER:
            std::cout << "NUMBER";
            break;
    }
    std::cout << std::endl;

    if (m->var.size()) {
        std::cout << "Variable: ";
        //m->var->Print();
    } 

    if (m->fun.size()) {
        std::cout << "Function: ";
        //m->fun->Print();
    } 

    std::cout << "Value: " << m->value << std::endl;
}

unsigned int _hash_(const string& str) {
    unsigned int h = 0;
    for (char c : str) {
        h = h * 65599 + c;
    }
    return h;
}

shared_ptr<bucket> create(const string& key, meta info, shared_ptr<bucket> next) {
    shared_ptr<bucket> b = make_shared<bucket>();
    b->key = key;
    b->info = info;
    b->next = next;
    return b;
}

void insert(const string& key, meta info, shared_ptr<bucket> table[]) {
    uint index = _hash_(key) % SIZE;
    shared_ptr<bucket> b = create(key, info, table[index]->next);
    table[index]->next = b;
}

meta lookup(const string& key, shared_ptr<bucket> table[]) {
    uint index = _hash_(key) % SIZE;
    shared_ptr<bucket> b;
    for (b = table[index]; b != nullptr; b = b->next) {
        if (b->key == key) {
            return b->info;
        }
    }
    return NULL;
}

int check_redef(const string& key, shared_ptr<bucket> table[]) {
    uint index = _hash_(key) % SIZE;
    shared_ptr<bucket> b;
    for (b = table[index]; b != nullptr && b->key != "*"; b = b->next) {
        if (b->key == key) {
            return false;
        }
    }
    return true;
}

int fun_redef(const string& key, shared_ptr<bucket> table[]) {
    uint index = _hash_(key) % SIZE;
    shared_ptr<bucket> b;
    for (b = table[index]; b != nullptr; b = b->next) {
        if (b->key == key) {
            return false;
        }
    }
    return true;
}

void pop(const string& key, shared_ptr<bucket> table[]) {
    uint index = _hash_(key) % SIZE;
    shared_ptr<bucket> temp = table[index]->next->next;
    table[index]->next.reset();
    table[index]->next = temp;
}

void beginscope(shared_ptr<bucket> table[]) {
    for (int i = 0; i < 109; i++) {
        shared_ptr<bucket> special = create("*", NULL, NULL);
        special->next = table[i]->next;
        table[i]->next = special;
    }
}

void endscope(shared_ptr<bucket> table[]) {
    for (int i = 0; i < 109; i++) {
        shared_ptr<bucket> head;
        for (head = table[i]->next; head != nullptr && head->key != "*";) {
            shared_ptr<bucket> temp = head->next;
            head.reset();
            head = temp;
            table[i]->next = head;
        }
        shared_ptr<bucket> temp = head->next;
        head.reset();
        head = temp;
        table[i]->next = head;
    }
}

// 创建值节点
BaseAST * createValueNode(int value, BaseAST * expr, BaseAST * exp) {
    auto node = new(InitValListAST);
    if (node != NULL) {
        node->next = NULL;
        node->isValue = 1;
        node->expr = expr;
        node->data.value = value;
        node->exp =  unique_ptr<BaseAST>(exp);
    }
    return node;
}

// 创建子列表节点
BaseAST * createChildNode(InitValListAST* child) {
    auto node = new(InitValListAST);
    if (node != NULL) {
        node->next = NULL;
        node->isValue = 0;
        node->data.child = child;
    }
    return node;
}

// 释放节点及其后续节点的内存
void freeInitList(InitValListAST* head) {
    while (head != NULL) {
        InitValListAST* next = head->next;
        if (!head->isValue) {
            freeInitList(head->data.child);
        }
        free(head);
        head = next;
    }
}

int zero_fill(InitValListAST* root, int size, int &index, int * val, int * array_size, int length){
      InitValListAST* head = root;
      InitValListAST* current = root->data.child;
      while(current){
            if(current->isValue){
                  val[index++] = current->data.value;
            }
            else 
            {
                  int t, mod;
                  for(t = 0, mod = 1; t < length; t++){
                        mod *= array_size[t];
                        if(t == length - 1) break;
                        else if(index % mod != 0) break;
                        //else if(index % mod != 0 || t == 0) t = -1;
                  }
                  //if(t == -1) return -1;
                  int new_index = index + mod / array_size[t];
                  zero_fill(current, new_index, index, val, array_size, t-1);
                  index = new_index;
                   
            }
            current = current->next;
      }
      printf("index : %d, size : %d\n" ,index, size);
      if(index > size) return -1;
      else return 0;
}


#line 289 "src/sysy.tab.cc"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "sysy.tab.hh"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_INT = 4,                        /* INT  */
  YYSYMBOL_ADD = 5,                        /* ADD  */
  YYSYMBOL_MUL = 6,                        /* MUL  */
  YYSYMBOL_SUB = 7,                        /* SUB  */
  YYSYMBOL_DIV = 8,                        /* DIV  */
  YYSYMBOL_MOD = 9,                        /* MOD  */
  YYSYMBOL_ASSIGN = 10,                    /* ASSIGN  */
  YYSYMBOL_NOT = 11,                       /* NOT  */
  YYSYMBOL_LE = 12,                        /* LE  */
  YYSYMBOL_GE = 13,                        /* GE  */
  YYSYMBOL_LT = 14,                        /* LT  */
  YYSYMBOL_GT = 15,                        /* GT  */
  YYSYMBOL_EQ = 16,                        /* EQ  */
  YYSYMBOL_NE = 17,                        /* NE  */
  YYSYMBOL_AND = 18,                       /* AND  */
  YYSYMBOL_OR = 19,                        /* OR  */
  YYSYMBOL_LBRACE = 20,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 21,                    /* RBRACE  */
  YYSYMBOL_LPAREN = 22,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 23,                    /* RPAREN  */
  YYSYMBOL_SEMICOLON = 24,                 /* SEMICOLON  */
  YYSYMBOL_LBRACKET = 25,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 26,                  /* RBRACKET  */
  YYSYMBOL_COMMA = 27,                     /* COMMA  */
  YYSYMBOL_AUTO = 28,                      /* AUTO  */
  YYSYMBOL_BREAK = 29,                     /* BREAK  */
  YYSYMBOL_CASE = 30,                      /* CASE  */
  YYSYMBOL_CHAR = 31,                      /* CHAR  */
  YYSYMBOL_CONST = 32,                     /* CONST  */
  YYSYMBOL_CONTINUE = 33,                  /* CONTINUE  */
  YYSYMBOL_DEFAULT = 34,                   /* DEFAULT  */
  YYSYMBOL_DO = 35,                        /* DO  */
  YYSYMBOL_DOUBLE = 36,                    /* DOUBLE  */
  YYSYMBOL_ELSE = 37,                      /* ELSE  */
  YYSYMBOL_ENUM = 38,                      /* ENUM  */
  YYSYMBOL_EXTERN = 39,                    /* EXTERN  */
  YYSYMBOL_FLOAT = 40,                     /* FLOAT  */
  YYSYMBOL_FOR = 41,                       /* FOR  */
  YYSYMBOL_GOTO = 42,                      /* GOTO  */
  YYSYMBOL_IF = 43,                        /* IF  */
  YYSYMBOL_INTTYPE = 44,                   /* INTTYPE  */
  YYSYMBOL_LONG = 45,                      /* LONG  */
  YYSYMBOL_REGISTER = 46,                  /* REGISTER  */
  YYSYMBOL_RETURN = 47,                    /* RETURN  */
  YYSYMBOL_SHORT = 48,                     /* SHORT  */
  YYSYMBOL_SIGNED = 49,                    /* SIGNED  */
  YYSYMBOL_SIZEOF = 50,                    /* SIZEOF  */
  YYSYMBOL_STATIC = 51,                    /* STATIC  */
  YYSYMBOL_STRUCT = 52,                    /* STRUCT  */
  YYSYMBOL_SWITCH = 53,                    /* SWITCH  */
  YYSYMBOL_TYPEDEF = 54,                   /* TYPEDEF  */
  YYSYMBOL_UNION = 55,                     /* UNION  */
  YYSYMBOL_UNSIGNED = 56,                  /* UNSIGNED  */
  YYSYMBOL_VOID = 57,                      /* VOID  */
  YYSYMBOL_VOLATILE = 58,                  /* VOLATILE  */
  YYSYMBOL_WHILE = 59,                     /* WHILE  */
  YYSYMBOL_YYACCEPT = 60,                  /* $accept  */
  YYSYMBOL_Start = 61,                     /* Start  */
  YYSYMBOL_CompUnit = 62,                  /* CompUnit  */
  YYSYMBOL_Decl = 63,                      /* Decl  */
  YYSYMBOL_VarDefList = 64,                /* VarDefList  */
  YYSYMBOL_VarDef = 65,                    /* VarDef  */
  YYSYMBOL_BRACKETList = 66,               /* BRACKETList  */
  YYSYMBOL_InitVal = 67,                   /* InitVal  */
  YYSYMBOL_InitValList = 68,               /* InitValList  */
  YYSYMBOL_FuncDef = 69,                   /* FuncDef  */
  YYSYMBOL_70_1 = 70,                      /* $@1  */
  YYSYMBOL_71_2 = 71,                      /* $@2  */
  YYSYMBOL_FuncFParamsOpt = 72,            /* FuncFParamsOpt  */
  YYSYMBOL_FuncFParams = 73,               /* FuncFParams  */
  YYSYMBOL_FuncFParam = 74,                /* FuncFParam  */
  YYSYMBOL_ArrayOpt = 75,                  /* ArrayOpt  */
  YYSYMBOL_BlockItems = 76,                /* BlockItems  */
  YYSYMBOL_BlockItem = 77,                 /* BlockItem  */
  YYSYMBOL_Stmt = 78,                      /* Stmt  */
  YYSYMBOL_MatchedStmt = 79,               /* MatchedStmt  */
  YYSYMBOL_OpenStmt = 80,                  /* OpenStmt  */
  YYSYMBOL_NonIfStmt = 81,                 /* NonIfStmt  */
  YYSYMBOL_82_3 = 82,                      /* $@3  */
  YYSYMBOL_OptExp = 83,                    /* OptExp  */
  YYSYMBOL_Exp = 84,                       /* Exp  */
  YYSYMBOL_Cond = 85,                      /* Cond  */
  YYSYMBOL_LVal = 86,                      /* LVal  */
  YYSYMBOL_PrimaryExp = 87,                /* PrimaryExp  */
  YYSYMBOL_Number = 88,                    /* Number  */
  YYSYMBOL_UnaryExp = 89,                  /* UnaryExp  */
  YYSYMBOL_UnaryOp = 90,                   /* UnaryOp  */
  YYSYMBOL_FuncRParamsOpt = 91,            /* FuncRParamsOpt  */
  YYSYMBOL_FuncRParams = 92,               /* FuncRParams  */
  YYSYMBOL_Arrays = 93,                    /* Arrays  */
  YYSYMBOL_MulExp = 94,                    /* MulExp  */
  YYSYMBOL_AddExp = 95,                    /* AddExp  */
  YYSYMBOL_RelExp = 96,                    /* RelExp  */
  YYSYMBOL_EqExp = 97,                     /* EqExp  */
  YYSYMBOL_LAndExp = 98,                   /* LAndExp  */
  YYSYMBOL_LOrExp = 99                     /* LOrExp  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   173

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  60
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  85
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  149

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   314


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   247,   247,   255,   264,   272,   279,   288,   298,   305,
     315,   324,   335,   339,   350,   361,   370,   376,   377,   388,
     387,   430,   429,   476,   481,   489,   496,   507,   523,   526,
     534,   539,   549,   559,   568,   575,   584,   593,   603,   611,
     623,   631,   639,   638,   649,   657,   667,   670,   676,   691,
     698,   709,   719,   729,   738,   740,   755,   765,   790,   791,
     792,   796,   801,   812,   830,   856,   860,   873,   887,   896,
     905,   916,   930,   939,   950,   964,   973,   982,   991,  1002,
    1016,  1025,  1036,  1050,  1061,  1075
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "INT", "ADD",
  "MUL", "SUB", "DIV", "MOD", "ASSIGN", "NOT", "LE", "GE", "LT", "GT",
  "EQ", "NE", "AND", "OR", "LBRACE", "RBRACE", "LPAREN", "RPAREN",
  "SEMICOLON", "LBRACKET", "RBRACKET", "COMMA", "AUTO", "BREAK", "CASE",
  "CHAR", "CONST", "CONTINUE", "DEFAULT", "DO", "DOUBLE", "ELSE", "ENUM",
  "EXTERN", "FLOAT", "FOR", "GOTO", "IF", "INTTYPE", "LONG", "REGISTER",
  "RETURN", "SHORT", "SIGNED", "SIZEOF", "STATIC", "STRUCT", "SWITCH",
  "TYPEDEF", "UNION", "UNSIGNED", "VOID", "VOLATILE", "WHILE", "$accept",
  "Start", "CompUnit", "Decl", "VarDefList", "VarDef", "BRACKETList",
  "InitVal", "InitValList", "FuncDef", "$@1", "$@2", "FuncFParamsOpt",
  "FuncFParams", "FuncFParam", "ArrayOpt", "BlockItems", "BlockItem",
  "Stmt", "MatchedStmt", "OpenStmt", "NonIfStmt", "$@3", "OptExp", "Exp",
  "Cond", "LVal", "PrimaryExp", "Number", "UnaryExp", "UnaryOp",
  "FuncRParamsOpt", "FuncRParams", "Arrays", "MulExp", "AddExp", "RelExp",
  "EqExp", "LAndExp", "LOrExp", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-116)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -29,     8,    16,    80,   -29,  -116,  -116,    59,     2,  -116,
      65,  -116,  -116,  -116,    42,     0,  -116,    85,    42,    87,
      75,    78,  -116,    97,   105,  -116,  -116,    88,    90,  -116,
      42,    91,  -116,  -116,  -116,  -116,    13,   126,  -116,  -116,
    -116,  -116,  -116,  -116,   126,    55,    64,    94,  -116,    95,
    -116,    96,  -116,   126,    93,  -116,  -116,    26,   101,  -116,
     126,   126,   126,   126,   126,  -116,   106,  -116,  -116,  -116,
     104,   107,   126,  -116,    97,  -116,  -116,  -116,  -116,    55,
      55,  -116,   103,     1,  -116,   126,   110,  -116,    35,  -116,
    -116,   116,    85,   126,   127,  -116,  -116,  -116,  -116,  -116,
    -116,   108,  -116,   146,  -116,  -116,  -116,  -116,   126,   133,
     126,  -116,   126,    63,   135,    64,   140,    20,   132,   141,
    -116,   136,   137,  -116,    92,   126,   126,   126,   126,   126,
     126,   126,   126,    92,  -116,  -116,   125,    64,    64,    64,
      64,   140,   140,    20,   132,  -116,    92,  -116,  -116
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     2,     5,     6,    12,     0,     8,
       0,     1,     3,     4,    23,    10,     7,     0,    23,     0,
       0,    24,    25,     0,     0,    12,     9,     0,    28,    19,
       0,    65,    54,    58,    59,    60,     0,     0,    11,    14,
      52,    55,    53,    67,     0,    71,    48,     0,    21,     0,
      27,     0,    26,    61,    50,    16,    17,     0,     0,    57,
       0,     0,     0,     0,     0,    13,     0,    12,    30,    63,
       0,    62,     0,    15,     0,    51,    68,    69,    70,    72,
      73,    30,    29,    46,    56,     0,     0,    18,    46,    42,
      20,     0,     0,    46,     0,    32,    31,    33,    34,    35,
      37,     0,    47,    52,    64,    66,    22,    30,     0,     0,
       0,    41,     0,    46,     0,    74,    79,    82,    84,    49,
      45,     0,     0,    43,    46,     0,     0,     0,     0,     0,
       0,     0,     0,    46,    40,    38,    34,    77,    78,    75,
      76,    80,    81,    83,    85,    44,    46,    36,    39
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -116,  -116,  -116,    54,  -116,   147,    98,   -33,  -116,   159,
    -116,  -116,   148,  -116,   138,  -116,   -80,  -116,   -74,  -115,
      21,  -116,  -116,    76,   -23,    60,   -81,  -116,  -116,    31,
    -116,  -116,  -116,  -116,     9,    15,   -53,    40,    41,  -116
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     3,     4,    95,     8,     9,    15,    38,    57,     6,
      51,    66,    20,    21,    22,    50,    83,    96,    97,    98,
      99,   100,   107,   101,   102,   114,    40,    41,    42,    43,
      44,    70,    71,    54,    45,    46,   116,   117,   118,   119
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      39,    88,   103,    56,    31,    32,    33,   103,    34,   136,
      23,     7,    35,    39,    58,     1,    31,    32,    33,    10,
      34,    89,    90,    37,    35,    24,    16,   113,     2,    17,
      69,   147,   103,    36,    55,    37,   129,   130,    31,    32,
      33,    87,    34,   103,    91,    92,    35,    73,    93,    86,
     135,    39,   103,    74,     5,    89,   106,    37,    12,   145,
      94,    60,   104,    61,    62,   103,    31,    32,    33,    63,
      34,    64,    79,    80,    35,    59,   141,   142,    91,    92,
      11,    14,    93,    89,   123,    37,    19,    18,    25,   122,
      28,    76,    77,    78,    94,    31,    32,    33,    29,    34,
      31,    32,    33,    35,    34,    30,    91,    92,    35,    47,
      93,    48,    89,    53,    37,    49,    68,    36,    72,    37,
      65,    67,    94,   115,    75,   115,    81,    84,    24,    31,
      32,    33,   111,    34,    85,    91,   105,    35,   108,    93,
     137,   138,   139,   140,   115,   115,   115,   115,    37,   110,
     131,    94,   125,   126,   127,   128,   112,   120,   124,   133,
     132,   134,   146,    13,    26,    82,    27,   148,    52,   109,
     121,   143,     0,   144
};

static const yytype_int16 yycheck[] =
{
      23,    81,    83,    36,     3,     4,     5,    88,     7,   124,
      10,     3,    11,    36,    37,    44,     3,     4,     5,     3,
       7,    20,    21,    22,    11,    25,    24,   107,    57,    27,
      53,   146,   113,    20,    21,    22,    16,    17,     3,     4,
       5,    74,     7,   124,    43,    44,    11,    21,    47,    72,
     124,    74,   133,    27,     0,    20,    21,    22,     4,   133,
      59,     6,    85,     8,     9,   146,     3,     4,     5,     5,
       7,     7,    63,    64,    11,    44,   129,   130,    43,    44,
       0,    22,    47,    20,    21,    22,    44,    22,     3,   112,
       3,    60,    61,    62,    59,     3,     4,     5,    23,     7,
       3,     4,     5,    11,     7,    27,    43,    44,    11,     4,
      47,    23,    20,    22,    22,    25,    20,    20,    25,    22,
      26,    26,    59,   108,    23,   110,    20,    23,    25,     3,
       4,     5,    24,     7,    27,    43,    26,    11,    22,    47,
     125,   126,   127,   128,   129,   130,   131,   132,    22,    22,
      18,    59,    12,    13,    14,    15,    10,    24,    23,    23,
      19,    24,    37,     4,    17,    67,    18,   146,    30,    93,
     110,   131,    -1,   132
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    44,    57,    61,    62,    63,    69,     3,    64,    65,
       3,     0,    63,    69,    22,    66,    24,    27,    22,    44,
      72,    73,    74,    10,    25,     3,    65,    72,     3,    23,
      27,     3,     4,     5,     7,    11,    20,    22,    67,    84,
      86,    87,    88,    89,    90,    94,    95,     4,    23,    25,
      75,    70,    74,    22,    93,    21,    67,    68,    84,    89,
       6,     8,     9,     5,     7,    26,    71,    26,    20,    84,
      91,    92,    25,    21,    27,    23,    89,    89,    89,    94,
      94,    20,    66,    76,    23,    27,    84,    67,    76,    20,
      21,    43,    44,    47,    59,    63,    77,    78,    79,    80,
      81,    83,    84,    86,    84,    26,    21,    82,    22,    83,
      22,    24,    10,    76,    85,    95,    96,    97,    98,    99,
      24,    85,    84,    21,    23,    12,    13,    14,    15,    16,
      17,    18,    19,    23,    24,    78,    79,    95,    95,    95,
      95,    96,    96,    97,    98,    78,    37,    79,    80
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    60,    61,    62,    62,    62,    62,    63,    64,    64,
      65,    65,    66,    66,    67,    67,    67,    68,    68,    70,
      69,    71,    69,    72,    72,    73,    73,    74,    75,    75,
      76,    76,    77,    77,    78,    78,    79,    79,    80,    80,
      81,    81,    82,    81,    81,    81,    83,    83,    84,    85,
      86,    87,    87,    87,    88,    89,    89,    89,    90,    90,
      90,    91,    91,    92,    92,    93,    93,    94,    94,    94,
      94,    95,    95,    95,    96,    96,    96,    96,    96,    97,
      97,    97,    98,    98,    99,    99
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     2,     1,     1,     3,     1,     3,
       2,     4,     0,     4,     1,     3,     2,     1,     3,     0,
       9,     0,     9,     0,     1,     1,     3,     3,     0,     3,
       0,     2,     1,     1,     1,     1,     7,     1,     5,     7,
       4,     2,     0,     4,     5,     3,     0,     1,     1,     1,
       2,     3,     1,     1,     1,     1,     4,     2,     1,     1,
       1,     0,     1,     1,     3,     0,     4,     1,     3,     3,
       3,     1,     3,     3,     1,     3,     3,     3,     3,     1,
       3,     3,     1,     3,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (AST, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, AST); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, std::unique_ptr<BaseAST> &AST)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (AST);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, std::unique_ptr<BaseAST> &AST)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, AST);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, std::unique_ptr<BaseAST> &AST)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], AST);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, AST); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, std::unique_ptr<BaseAST> &AST)
{
  YY_USE (yyvaluep);
  YY_USE (AST);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (std::unique_ptr<BaseAST> &AST)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* Start: CompUnit  */
#line 248 "src/sysy.y"
            {
                  auto root = make_unique<StartAST>();
                  root->comp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
                  AST = move(root);
                  //fprintf(yyout, "//Intermediate code generated.\n");
            }
#line 1493 "src/sysy.tab.cc"
    break;

  case 3: /* CompUnit: CompUnit Decl  */
#line 256 "src/sysy.y"
            {
                  auto comp_unit = new CompUnitAST();
                  comp_unit->kind = COMP_DEL;
                  comp_unit->comp = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
                  comp_unit->decl = unique_ptr<BaseAST>((yyvsp[0].ast_val));
                  (yyval.ast_val) = comp_unit;
                  
            }
#line 1506 "src/sysy.tab.cc"
    break;

  case 4: /* CompUnit: CompUnit FuncDef  */
#line 265 "src/sysy.y"
            {
                  auto comp_unit = new CompUnitAST();
                  comp_unit->kind = COMP_FUNC_DEF;
                  comp_unit->comp = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
                  comp_unit->func_def = unique_ptr<BaseAST>((yyvsp[0].ast_val));
                  (yyval.ast_val) = comp_unit;
            }
#line 1518 "src/sysy.tab.cc"
    break;

  case 5: /* CompUnit: Decl  */
#line 273 "src/sysy.y"
            {
                  auto comp_unit = new CompUnitAST();
                  comp_unit->kind = DECL;
                  comp_unit->decl = unique_ptr<BaseAST>((yyvsp[0].ast_val));
                  (yyval.ast_val) = comp_unit;
            }
#line 1529 "src/sysy.tab.cc"
    break;

  case 6: /* CompUnit: FuncDef  */
#line 280 "src/sysy.y"
            {
                  auto comp_unit = new CompUnitAST();
                  comp_unit->kind = FUNC_DEF;
                  comp_unit->func_def = unique_ptr<BaseAST>((yyvsp[0].ast_val));
                  (yyval.ast_val) = comp_unit;
            }
#line 1540 "src/sysy.tab.cc"
    break;

  case 7: /* Decl: INTTYPE VarDefList SEMICOLON  */
#line 289 "src/sysy.y"
            {
                  auto ast = new DeclAST();
                  ast->var_type = "INT";
                  ast->vardeflist = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
                  ast->is_global = 1;
                  (yyval.ast_val) = ast;
            }
#line 1552 "src/sysy.tab.cc"
    break;

  case 8: /* VarDefList: VarDef  */
#line 299 "src/sysy.y"
            {
                  auto ast = new VarDefListAST();
                  ast->is_single = 1;
                  ast->vardef = unique_ptr<BaseAST>((yyvsp[0].ast_val));
                  (yyval.ast_val) = ast;
            }
#line 1563 "src/sysy.tab.cc"
    break;

  case 9: /* VarDefList: VarDefList COMMA VarDef  */
#line 306 "src/sysy.y"
            {
                  auto ast = new VarDefListAST();
                  ast->is_single = 0;
                  ast->vardeflist = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
                  ast->vardef = unique_ptr<BaseAST>((yyvsp[0].ast_val));
                  (yyval.ast_val) = ast;
            }
#line 1575 "src/sysy.tab.cc"
    break;

  case 10: /* VarDef: ID BRACKETList  */
#line 316 "src/sysy.y"
            {
                  auto ast = new VarDefAST();
                  ast->is_init = 0;
                  ast->ident = *unique_ptr<string>((yyvsp[-1].str_val));
                  ast->bracketlist = unique_ptr<BaseAST>((yyvsp[0].ast_val));
                  (yyval.ast_val) = ast;

            }
#line 1588 "src/sysy.tab.cc"
    break;

  case 11: /* VarDef: ID BRACKETList ASSIGN InitVal  */
#line 325 "src/sysy.y"
            {
                  auto ast = new VarDefAST();
                  ast->is_init = 1;
                  ast->ident = *unique_ptr<string>((yyvsp[-3].str_val));
                  ast->bracketlist = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
                  ast->initval = unique_ptr<BaseAST>((yyvsp[0].ast_val));
                  (yyval.ast_val) = ast;
            }
#line 1601 "src/sysy.tab.cc"
    break;

  case 12: /* BRACKETList: %empty  */
#line 335 "src/sysy.y"
            {
                  auto ast = new BRACKETListAST();
                  (yyval.ast_val) = ast;
            }
#line 1610 "src/sysy.tab.cc"
    break;

  case 13: /* BRACKETList: BRACKETList LBRACKET INT RBRACKET  */
#line 340 "src/sysy.y"
            {
                  auto ast = new BRACKETListAST();
                  int v = (yyvsp[-1].int_val);
                  ast->bracketlist = dynamic_cast<BRACKETListAST*>((yyvsp[-3].ast_val))->bracketlist;
                  ast->bracketlist.push_back(v);
                  ast->is_arrays = 0;
                  (yyval.ast_val) = ast;
            }
#line 1623 "src/sysy.tab.cc"
    break;

  case 14: /* InitVal: Exp  */
#line 351 "src/sysy.y"
      {
            // auto ast = new InitValAST();
            // ast->is_exp = 1;
            // ast->exp = unique_ptr<BaseAST>($1);
            ExpAST * t = dynamic_cast<ExpAST *>((yyvsp[0].ast_val));
            // ast->value = t->value;
            // $$ = ast;
            (yyval.ast_val) = createValueNode(t->value, (yyvsp[0].ast_val), (yyvsp[0].ast_val));
            
      }
#line 1638 "src/sysy.tab.cc"
    break;

  case 15: /* InitVal: LBRACE InitValList RBRACE  */
#line 362 "src/sysy.y"
      {
            // auto ast = new InitValAST();
            // ast->is_exp = 0;
            // ast->initvallist = unique_ptr<BaseAST>($2);
            // $$ = ast;
            InitValListAST * pt = dynamic_cast<InitValListAST *>((yyvsp[-1].ast_val));
            (yyval.ast_val) = createChildNode(pt);
      }
#line 1651 "src/sysy.tab.cc"
    break;

  case 16: /* InitVal: LBRACE RBRACE  */
#line 371 "src/sysy.y"
      {
            (yyval.ast_val) = createChildNode(NULL);
      }
#line 1659 "src/sysy.tab.cc"
    break;

  case 17: /* InitValList: InitVal  */
#line 376 "src/sysy.y"
                       {(yyval.ast_val) = (yyvsp[0].ast_val);}
#line 1665 "src/sysy.tab.cc"
    break;

  case 18: /* InitValList: InitValList COMMA InitVal  */
#line 378 "src/sysy.y"
                {
                  InitValListAST * tail, * t = dynamic_cast<InitValListAST *>((yyvsp[-2].ast_val));
                  for(tail = t; tail->next; tail = tail->next);
                  InitValListAST * pt = dynamic_cast<InitValListAST *>((yyvsp[0].ast_val));
                  tail->next = pt;
                  (yyval.ast_val) = t;
                }
#line 1677 "src/sysy.tab.cc"
    break;

  case 19: /* $@1: %empty  */
#line 388 "src/sysy.y"
      {
            // FuncFParamsOptAST * t1 = dynamic_cast<FuncFParamsOptAST*>($4);
            // FuncFParamsAST * t = dynamic_cast<FuncFParamsAST*>(t1->funparams.get());
            // if(t!=NULL){
            //       for (const auto& param : t->funparams) {
            //             vector<int> var = param.bracketlist;
            //             meta m = init_meta(FUNC_PARAM, var, {}, 0);
            //             if(check_redef(param.ident, table_parse) == false){
            //             yyerror(AST, "param redefine error");
            //       }
            //       else insert(param.ident, m, table_parse);
            //       }
            // }
      }
#line 1696 "src/sysy.tab.cc"
    break;

  case 20: /* FuncDef: INTTYPE ID LPAREN FuncFParamsOpt RPAREN $@1 LBRACE BlockItems RBRACE  */
#line 403 "src/sysy.y"
      {     
            auto ast = new FuncDefAST();
            ast->fun_type = "INT";
            ast->ident = *unique_ptr<string>((yyvsp[-7].str_val));
            ast->params = unique_ptr<BaseAST>((yyvsp[-5].ast_val));
            ast->block = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
            (yyval.ast_val) = ast;

            // FuncFParamsOptAST * t1 = dynamic_cast<FuncFParamsOptAST*>($4);
            // FuncFParamsAST * t = dynamic_cast<FuncFParamsAST*>(t1->funparams.get());
            
            // std::vector<Param> fun;
            // if(t!=NULL){
            //       for (const auto& param : t->funparams) {
            //             Param elm = Param(param.is_lval, param.ident, param.value, param.bracketlist);
            //             fun.push_back(elm);
            //       }
            // }
            // meta m = init_meta(FUNCTION_INT, {}, fun, 0);

            // if(fun_redef(ast->ident, table_parse) == false){
            //       yyerror(AST, "func redefine error");
            // }
            // else insert(ast->ident, m, table_parse);
            // print_meta(m);
      }
#line 1727 "src/sysy.tab.cc"
    break;

  case 21: /* $@2: %empty  */
#line 430 "src/sysy.y"
        {
            {
            // FuncFParamsOptAST * t1 = dynamic_cast<FuncFParamsOptAST*>($4);
            // FuncFParamsAST * t = dynamic_cast<FuncFParamsAST*>(t1->funparams.get());
            // if(t!=NULL){
            //       for (const auto& param : t->funparams) {
            //             vector<int> var = param.bracketlist;
            //             meta m = init_meta(FUNC_PARAM, var, {}, 0);
            //             if(check_redef(param.ident, table_parse) == false){
            //             yyerror(AST, "param redefine error");
            //       }
            //       else insert(param.ident, m, table_parse);
            //       }
            // }
      }
        }
#line 1748 "src/sysy.tab.cc"
    break;

  case 22: /* FuncDef: VOID ID LPAREN FuncFParamsOpt RPAREN $@2 LBRACE BlockItems RBRACE  */
#line 447 "src/sysy.y"
        {
            auto ast = new FuncDefAST();
            ast->fun_type = "VOID";
            ast->ident = *unique_ptr<string>((yyvsp[-7].str_val));
            ast->params = unique_ptr<BaseAST>((yyvsp[-5].ast_val));
            ast->block = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
            (yyval.ast_val) = ast;

            // FuncFParamsOptAST * t1 = dynamic_cast<FuncFParamsOptAST*>($4);
            // FuncFParamsAST * t = dynamic_cast<FuncFParamsAST*>(t1->funparams.get());
            // std::vector<Param> fun;
            // if(t!=NULL){
            //       for (const auto& param : t->funparams) {
            //             Param elm = Param(param.is_lval, param.ident, param.value, param.bracketlist);
            //             fun.push_back(elm);
            //       }
            // }
            // meta m = init_meta(FUNCTION_VOID, {}, fun, 0);

            // if(fun_redef(ast->ident, table_parse) == false){
            //       yyerror(AST, "func redefine error");
            // }
            // else insert(ast->ident, m, table_parse);
            // print_meta(m);
        }
#line 1778 "src/sysy.tab.cc"
    break;

  case 23: /* FuncFParamsOpt: %empty  */
#line 476 "src/sysy.y"
      {
            auto ast = new FuncFParamsOptAST();
            ast->funparams = NULL;
            (yyval.ast_val) = ast;
      }
#line 1788 "src/sysy.tab.cc"
    break;

  case 24: /* FuncFParamsOpt: FuncFParams  */
#line 482 "src/sysy.y"
      {
            auto ast = new FuncFParamsOptAST();
            ast->funparams = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            (yyval.ast_val) = ast;
      }
#line 1798 "src/sysy.tab.cc"
    break;

  case 25: /* FuncFParams: FuncFParam  */
#line 490 "src/sysy.y"
      {
            auto ast = new FuncFParamsAST();
            FuncParamAST * t = dynamic_cast<FuncParamAST*>((yyvsp[0].ast_val));
            ast->funparams.push_back(*t);
            (yyval.ast_val) = ast;
      }
#line 1809 "src/sysy.tab.cc"
    break;

  case 26: /* FuncFParams: FuncFParams COMMA FuncFParam  */
#line 497 "src/sysy.y"
      {
            auto ast = new FuncFParamsAST();
            FuncFParamsAST * t = dynamic_cast<FuncFParamsAST*>((yyvsp[-2].ast_val));
            FuncParamAST * p = dynamic_cast<FuncParamAST*>((yyvsp[0].ast_val));
            ast->funparams = t->funparams;
            ast->funparams.push_back(*p);
            (yyval.ast_val) = ast;
      }
#line 1822 "src/sysy.tab.cc"
    break;

  case 27: /* FuncFParam: INTTYPE ID ArrayOpt  */
#line 508 "src/sysy.y"
      {
            auto ast = new FuncParamAST();
            ast->ident = *unique_ptr<string>((yyvsp[-1].str_val));
            ast->is_lval = 1;
            if((yyvsp[0].ast_val) != NULL){
                  BRACKETListAST * t = dynamic_cast<BRACKETListAST*>((yyvsp[0].ast_val));
                  ast->bracketlist = t->bracketlist;
                  ast->bracketlist.insert(ast->bracketlist.begin(), 2003);
                  //std::cout<<"list size : " << ast->bracketlist.size() << endl;
            }
            (yyval.ast_val) = ast;
      }
#line 1839 "src/sysy.tab.cc"
    break;

  case 28: /* ArrayOpt: %empty  */
#line 523 "src/sysy.y"
      {
            (yyval.ast_val) = NULL;
      }
#line 1847 "src/sysy.tab.cc"
    break;

  case 29: /* ArrayOpt: LBRACKET RBRACKET BRACKETList  */
#line 527 "src/sysy.y"
      {
            (yyval.ast_val) = (yyvsp[0].ast_val);
      }
#line 1855 "src/sysy.tab.cc"
    break;

  case 30: /* BlockItems: %empty  */
#line 534 "src/sysy.y"
      {
            auto ast = new BlockItemsAST();
            ast->empty = 1;
            (yyval.ast_val) = ast;
      }
#line 1865 "src/sysy.tab.cc"
    break;

  case 31: /* BlockItems: BlockItems BlockItem  */
#line 540 "src/sysy.y"
      {
            auto ast = new BlockItemsAST();
            ast->blockitems = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
            ast->block = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->empty = 0;
            (yyval.ast_val) = ast;
      }
#line 1877 "src/sysy.tab.cc"
    break;

  case 32: /* BlockItem: Decl  */
#line 550 "src/sysy.y"
      {
            auto ast = new BlockItemAST();
            ast->is_decl = 1;
            ast->decl = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            DeclAST * t = dynamic_cast<DeclAST *>(ast->decl.get());
            t->is_global = 0;
            cout << "IS Global : "<< t->is_global <<endl;
            (yyval.ast_val) = ast;
      }
#line 1891 "src/sysy.tab.cc"
    break;

  case 33: /* BlockItem: Stmt  */
#line 560 "src/sysy.y"
      {
            auto ast = new BlockItemAST();
            ast->is_decl = 0;
            ast->stmt = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            (yyval.ast_val) = ast;
      }
#line 1902 "src/sysy.tab.cc"
    break;

  case 34: /* Stmt: MatchedStmt  */
#line 569 "src/sysy.y"
      {
            auto ast = new StmtAST();
            ast->is_open = 0;
            ast->matched = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            (yyval.ast_val) = ast;
      }
#line 1913 "src/sysy.tab.cc"
    break;

  case 35: /* Stmt: OpenStmt  */
#line 576 "src/sysy.y"
      {
            auto ast = new StmtAST();
            ast->is_open = 1;
            ast->open = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            (yyval.ast_val) = ast;
      }
#line 1924 "src/sysy.tab.cc"
    break;

  case 36: /* MatchedStmt: IF LPAREN Cond RPAREN MatchedStmt ELSE MatchedStmt  */
#line 585 "src/sysy.y"
      {
            auto ast = new MatchedStmtAST();
            ast->is_if = 1;
            ast->cond = unique_ptr<BaseAST>((yyvsp[-4].ast_val));
            ast->matched1 = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->matched2 = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            (yyval.ast_val) = ast;
      }
#line 1937 "src/sysy.tab.cc"
    break;

  case 37: /* MatchedStmt: NonIfStmt  */
#line 594 "src/sysy.y"
      {
            auto ast = new MatchedStmtAST();
            ast->is_if = 0;
            ast->noif = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            (yyval.ast_val) = ast;
      }
#line 1948 "src/sysy.tab.cc"
    break;

  case 38: /* OpenStmt: IF LPAREN Cond RPAREN Stmt  */
#line 604 "src/sysy.y"
      {
            auto ast = new OpenStmtAST();
            ast->is_match = 0;
            ast->cond = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->stmt = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            (yyval.ast_val) = ast;
      }
#line 1960 "src/sysy.tab.cc"
    break;

  case 39: /* OpenStmt: IF LPAREN Cond RPAREN MatchedStmt ELSE OpenStmt  */
#line 612 "src/sysy.y"
      {
            auto ast = new OpenStmtAST();
            ast->is_match = 1;
            ast->cond = unique_ptr<BaseAST>((yyvsp[-4].ast_val));
            ast->matched = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->open = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            (yyval.ast_val) = ast;
      }
#line 1973 "src/sysy.tab.cc"
    break;

  case 40: /* NonIfStmt: LVal ASSIGN Exp SEMICOLON  */
#line 624 "src/sysy.y"
      {
            auto ast = new NonIfStmtAST();
            ast->type = lv;
            ast->lval = unique_ptr<BaseAST>((yyvsp[-3].ast_val));
            ast->exp = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
            (yyval.ast_val) = ast;
      }
#line 1985 "src/sysy.tab.cc"
    break;

  case 41: /* NonIfStmt: OptExp SEMICOLON  */
#line 632 "src/sysy.y"
      {
            auto ast = new NonIfStmtAST();
            ast->type = opt;
            ast->exp = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
            (yyval.ast_val) = ast;
      }
#line 1996 "src/sysy.tab.cc"
    break;

  case 42: /* $@3: %empty  */
#line 639 "src/sysy.y"
    {
      //beginscope();
      }
#line 2004 "src/sysy.tab.cc"
    break;

  case 43: /* NonIfStmt: LBRACE $@3 BlockItems RBRACE  */
#line 642 "src/sysy.y"
      {
            //endscope();
            auto ast = new NonIfStmtAST();
            ast->type = lbr;
            ast->blockitems = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
            (yyval.ast_val) = ast;
      }
#line 2016 "src/sysy.tab.cc"
    break;

  case 44: /* NonIfStmt: WHILE LPAREN Cond RPAREN Stmt  */
#line 650 "src/sysy.y"
      {
            auto ast = new NonIfStmtAST();
            ast->type = whi;
            ast->cond = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->stmt = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            (yyval.ast_val) = ast;
      }
#line 2028 "src/sysy.tab.cc"
    break;

  case 45: /* NonIfStmt: RETURN OptExp SEMICOLON  */
#line 658 "src/sysy.y"
      {
            auto ast = new NonIfStmtAST();
            ast->type = ret;
            ast->exp = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
            (yyval.ast_val) = ast;
      }
#line 2039 "src/sysy.tab.cc"
    break;

  case 46: /* OptExp: %empty  */
#line 667 "src/sysy.y"
      {
            (yyval.ast_val) = NULL;
      }
#line 2047 "src/sysy.tab.cc"
    break;

  case 47: /* OptExp: Exp  */
#line 671 "src/sysy.y"
      {
            (yyval.ast_val) = (yyvsp[0].ast_val);
      }
#line 2055 "src/sysy.tab.cc"
    break;

  case 48: /* Exp: AddExp  */
#line 677 "src/sysy.y"
      {
            auto ast = new ExpAST();
            ast->addexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            AddExpAST* t = dynamic_cast<AddExpAST*>((yyvsp[0].ast_val));
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            (yyval.ast_val) = ast;
            
      }
#line 2073 "src/sysy.tab.cc"
    break;

  case 49: /* Cond: LOrExp  */
#line 692 "src/sysy.y"
      {
            auto ast = new CondAST();
            ast->lorexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            (yyval.ast_val) = ast;
      }
#line 2083 "src/sysy.tab.cc"
    break;

  case 50: /* LVal: ID Arrays  */
#line 699 "src/sysy.y"
      {
            auto ast = new FuncParamAST();
            ast->ident = *unique_ptr<string>((yyvsp[-1].str_val));
            BRACKETListAST * t = dynamic_cast<BRACKETListAST*>((yyvsp[0].ast_val));
            ast->bracketlist = t->bracketlist;
            ast->explist = t->explist;
            ast->is_arrays = t->is_arrays;
            (yyval.ast_val) = ast;
      }
#line 2097 "src/sysy.tab.cc"
    break;

  case 51: /* PrimaryExp: LPAREN Exp RPAREN  */
#line 710 "src/sysy.y"
      {
            auto ast = new PrimaryExpAST();
            ast->type = EXP;
            ast->exp = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
            ast->value = dynamic_cast<ExpAST*>((yyvsp[-1].ast_val))->value;
            ast->ident = dynamic_cast<ExpAST*>((yyvsp[-1].ast_val))->ident;
            ast->bracketlist = dynamic_cast<ExpAST*>((yyvsp[-1].ast_val))->bracketlist;
            (yyval.ast_val) = ast;
      }
#line 2111 "src/sysy.tab.cc"
    break;

  case 52: /* PrimaryExp: LVal  */
#line 720 "src/sysy.y"
      {
            auto ast = new PrimaryExpAST();
            ast->type = Lval;
            FuncParamAST * t = dynamic_cast<FuncParamAST*>((yyvsp[0].ast_val));
            ast->ident = t->ident;
            ast->bracketlist = t->bracketlist;
            ast->explist = t->explist;
            (yyval.ast_val) = ast;
      }
#line 2125 "src/sysy.tab.cc"
    break;

  case 53: /* PrimaryExp: Number  */
#line 730 "src/sysy.y"
      {
            auto ast = new PrimaryExpAST();
            ast->type = Num;
            ast->value = (yyvsp[0].int_val);
            (yyval.ast_val) = ast;
      }
#line 2136 "src/sysy.tab.cc"
    break;

  case 54: /* Number: INT  */
#line 738 "src/sysy.y"
           {(yyval.int_val) = (yyvsp[0].int_val);}
#line 2142 "src/sysy.tab.cc"
    break;

  case 55: /* UnaryExp: PrimaryExp  */
#line 741 "src/sysy.y"
      {
            auto ast = new UnaryExpAST();
            PrimaryExpAST* t = dynamic_cast<PrimaryExpAST*>((yyvsp[0].ast_val));
            if(t->type == Lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            
            ast->primaryexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->op = NONE;
            (yyval.ast_val) = ast;
      }
#line 2161 "src/sysy.tab.cc"
    break;

  case 56: /* UnaryExp: ID LPAREN FuncRParamsOpt RPAREN  */
#line 756 "src/sysy.y"
      {
            auto ast = new UnaryExpAST();
            auto fun = new FunCallAST();
            fun->ident = *unique_ptr<string>((yyvsp[-3].str_val));
            fun->funparams = unique_ptr<BaseAST>((yyvsp[-1].ast_val));
            ast->funcall = unique_ptr<BaseAST>(fun);
            ast->op = FUNCALL;
            (yyval.ast_val) = ast;
      }
#line 2175 "src/sysy.tab.cc"
    break;

  case 57: /* UnaryExp: UnaryOp UnaryExp  */
#line 766 "src/sysy.y"
      {
            auto ast = new UnaryExpAST();
            switch ((yyvsp[-1].int_val)) {
            case Add:
                  ast->value = + dynamic_cast<UnaryExpAST*>((yyvsp[0].ast_val))->value;
                  ast->op = Add;
                  break;
            case Sub:
                  ast->value = - dynamic_cast<UnaryExpAST*>((yyvsp[0].ast_val))->value;
                  ast->op = Sub;
                  break;
            case Not:
                  ast->value = ! dynamic_cast<UnaryExpAST*>((yyvsp[0].ast_val))->value;
                  ast->op = Not;
                  break;
            default : break;
      }
            std::string id = dynamic_cast<UnaryExpAST*>((yyvsp[0].ast_val))->ident;
            ast->unaryexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            
            (yyval.ast_val) = ast;
      }
#line 2202 "src/sysy.tab.cc"
    break;

  case 58: /* UnaryOp: ADD  */
#line 790 "src/sysy.y"
            {(yyval.int_val) = Add;}
#line 2208 "src/sysy.tab.cc"
    break;

  case 59: /* UnaryOp: SUB  */
#line 791 "src/sysy.y"
            {(yyval.int_val) = Sub;}
#line 2214 "src/sysy.tab.cc"
    break;

  case 60: /* UnaryOp: NOT  */
#line 792 "src/sysy.y"
            {(yyval.int_val) = Not;}
#line 2220 "src/sysy.tab.cc"
    break;

  case 61: /* FuncRParamsOpt: %empty  */
#line 796 "src/sysy.y"
      {
            auto ast = new FuncFParamsOptAST();
            ast->funparams = NULL;
            (yyval.ast_val) = ast;
      }
#line 2230 "src/sysy.tab.cc"
    break;

  case 62: /* FuncRParamsOpt: FuncRParams  */
#line 802 "src/sysy.y"
      {
            auto ast = new FuncFParamsOptAST();
            ast->funparams = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            FuncFParamsAST * t = dynamic_cast<FuncFParamsAST*>((yyvsp[0].ast_val));
            //std::cout << "length : "<< t->funparams.size()<<endl;
            
            (yyval.ast_val) = ast;
      }
#line 2243 "src/sysy.tab.cc"
    break;

  case 63: /* FuncRParams: Exp  */
#line 813 "src/sysy.y"
      {
            auto ast = new FuncFParamsAST();
            auto e = new FuncParamAST();
            ExpAST * t = dynamic_cast<ExpAST*>((yyvsp[0].ast_val));
            if(t->is_lval){
                  e->is_lval = 1;
                  e->ident = t->ident;
                  e->bracketlist = t->bracketlist;
            }
            else{
                  e->is_lval = 0;
                  e->value = t->value;
            }
            e->exp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->funparams.push_back(*e);
            (yyval.ast_val) = ast;
      }
#line 2265 "src/sysy.tab.cc"
    break;

  case 64: /* FuncRParams: FuncRParams COMMA Exp  */
#line 831 "src/sysy.y"
      {
            auto ast = new FuncFParamsAST();
            FuncFParamsAST * t = dynamic_cast<FuncFParamsAST*>((yyvsp[-2].ast_val));
            ast->funparams = t->funparams;
            auto e = new FuncParamAST();
            ExpAST * p = dynamic_cast<ExpAST*>((yyvsp[0].ast_val));
            
            if(p->is_lval){
                  e->is_lval = 1;
                  e->ident = p->ident;
                  e->bracketlist = p->bracketlist;
            }
            else{
                  e->is_lval = 0;
                  e->value = p->value;
            }
            e->exp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->funparams.push_back(*e);
            (yyval.ast_val) = ast;
            
      }
#line 2291 "src/sysy.tab.cc"
    break;

  case 65: /* Arrays: %empty  */
#line 856 "src/sysy.y"
      {
            auto ast = new BRACKETListAST();
            (yyval.ast_val) = ast;
      }
#line 2300 "src/sysy.tab.cc"
    break;

  case 66: /* Arrays: Arrays LBRACKET Exp RBRACKET  */
#line 861 "src/sysy.y"
        {
            auto ast = new BRACKETListAST();
            //int v = dynamic_cast<ExpAST*>($3)->value;
            ast->bracketlist = dynamic_cast<BRACKETListAST*>((yyvsp[-3].ast_val))->bracketlist;
            ast->explist = dynamic_cast<BRACKETListAST*>((yyvsp[-3].ast_val))->explist;
            ast->explist.push_back((yyvsp[-1].ast_val));
            ast->is_arrays = 1;
            (yyval.ast_val) = ast;
        }
#line 2314 "src/sysy.tab.cc"
    break;

  case 67: /* MulExp: UnaryExp  */
#line 874 "src/sysy.y"
      {
            auto ast = new MulExpAST();
            ast->unaryexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            UnaryExpAST* t = dynamic_cast<UnaryExpAST*>((yyvsp[0].ast_val));
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            (yyval.ast_val) = ast;
      }
#line 2332 "src/sysy.tab.cc"
    break;

  case 68: /* MulExp: MulExp MUL UnaryExp  */
#line 888 "src/sysy.y"
      {
            auto ast = new MulExpAST();
            ast->mulexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->unaryexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->value = dynamic_cast<MulExpAST*>((yyvsp[-2].ast_val))->value * dynamic_cast<UnaryExpAST*>((yyvsp[0].ast_val))->value;
            ast->op = Mul;
            (yyval.ast_val) = ast;
      }
#line 2345 "src/sysy.tab.cc"
    break;

  case 69: /* MulExp: MulExp DIV UnaryExp  */
#line 897 "src/sysy.y"
      {
            auto ast = new MulExpAST();
            ast->mulexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->unaryexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            //ast->value = dynamic_cast<MulExpAST*>($1)->value / dynamic_cast<UnaryExpAST*>($3)->value;
            ast->op = Div;
            (yyval.ast_val) = ast;
      }
#line 2358 "src/sysy.tab.cc"
    break;

  case 70: /* MulExp: MulExp MOD UnaryExp  */
#line 906 "src/sysy.y"
      {
            auto ast = new MulExpAST();
            ast->mulexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->unaryexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            //ast->value = dynamic_cast<MulExpAST*>($1)->value % dynamic_cast<UnaryExpAST*>($3)->value;
            ast->op = Mod;
            (yyval.ast_val) = ast;
      }
#line 2371 "src/sysy.tab.cc"
    break;

  case 71: /* AddExp: MulExp  */
#line 917 "src/sysy.y"
      {
            auto ast = new AddExpAST();
            ast->mulexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            MulExpAST* t = dynamic_cast<MulExpAST*>((yyvsp[0].ast_val));
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            (yyval.ast_val) = ast;
      }
#line 2389 "src/sysy.tab.cc"
    break;

  case 72: /* AddExp: AddExp ADD MulExp  */
#line 931 "src/sysy.y"
      {
            auto ast = new AddExpAST();
            ast->addexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->mulexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->value = dynamic_cast<AddExpAST*>((yyvsp[-2].ast_val))->value + dynamic_cast<MulExpAST*>((yyvsp[0].ast_val))->value;
            ast->op = Add;
            (yyval.ast_val) = ast;
      }
#line 2402 "src/sysy.tab.cc"
    break;

  case 73: /* AddExp: AddExp SUB MulExp  */
#line 940 "src/sysy.y"
      {
            auto ast = new AddExpAST();
            ast->addexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->mulexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->value = dynamic_cast<AddExpAST*>((yyvsp[-2].ast_val))->value - dynamic_cast<MulExpAST*>((yyvsp[0].ast_val))->value;
            ast->op = Sub;
            (yyval.ast_val) = ast;
      }
#line 2415 "src/sysy.tab.cc"
    break;

  case 74: /* RelExp: AddExp  */
#line 951 "src/sysy.y"
      {
            auto ast = new RelExpAST();
            ast->addexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            AddExpAST* t = dynamic_cast<AddExpAST*>((yyvsp[0].ast_val));
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            (yyval.ast_val) = ast;
      }
#line 2433 "src/sysy.tab.cc"
    break;

  case 75: /* RelExp: RelExp LT AddExp  */
#line 965 "src/sysy.y"
      {
            auto ast = new RelExpAST();
            ast->relexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->addexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->value = dynamic_cast<RelExpAST*>((yyvsp[-2].ast_val))->value < dynamic_cast<AddExpAST*>((yyvsp[0].ast_val))->value;
            ast->op = Lt;
            (yyval.ast_val) = ast;
      }
#line 2446 "src/sysy.tab.cc"
    break;

  case 76: /* RelExp: RelExp GT AddExp  */
#line 974 "src/sysy.y"
      {
            auto ast = new RelExpAST();
            ast->relexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->addexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->value = dynamic_cast<RelExpAST*>((yyvsp[-2].ast_val))->value > dynamic_cast<AddExpAST*>((yyvsp[0].ast_val))->value;
            ast->op = Gt;
            (yyval.ast_val) = ast;
      }
#line 2459 "src/sysy.tab.cc"
    break;

  case 77: /* RelExp: RelExp LE AddExp  */
#line 983 "src/sysy.y"
      {
            auto ast = new RelExpAST();
            ast->relexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->addexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->value = dynamic_cast<RelExpAST*>((yyvsp[-2].ast_val))->value <= dynamic_cast<AddExpAST*>((yyvsp[0].ast_val))->value;
            ast->op = Le;
            (yyval.ast_val) = ast;
      }
#line 2472 "src/sysy.tab.cc"
    break;

  case 78: /* RelExp: RelExp GE AddExp  */
#line 992 "src/sysy.y"
      {
            auto ast = new RelExpAST();
            ast->relexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->addexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->value = dynamic_cast<RelExpAST*>((yyvsp[-2].ast_val))->value >= dynamic_cast<AddExpAST*>((yyvsp[0].ast_val))->value;
            ast->op = Ge;
            (yyval.ast_val) = ast;
      }
#line 2485 "src/sysy.tab.cc"
    break;

  case 79: /* EqExp: RelExp  */
#line 1003 "src/sysy.y"
      {
            auto ast = new EqExpAST();
            ast->relexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            RelExpAST* t = dynamic_cast<RelExpAST*>((yyvsp[0].ast_val));
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            (yyval.ast_val) = ast;
      }
#line 2503 "src/sysy.tab.cc"
    break;

  case 80: /* EqExp: EqExp EQ RelExp  */
#line 1017 "src/sysy.y"
     {
            auto ast = new EqExpAST();
            ast->eqexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->relexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->value = dynamic_cast<EqExpAST*>((yyvsp[-2].ast_val))->value == dynamic_cast<RelExpAST*>((yyvsp[0].ast_val))->value;
            ast->op = Eq;
            (yyval.ast_val) = ast;
      }
#line 2516 "src/sysy.tab.cc"
    break;

  case 81: /* EqExp: EqExp NE RelExp  */
#line 1026 "src/sysy.y"
     {
            auto ast = new EqExpAST();
            ast->eqexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->relexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->value = dynamic_cast<EqExpAST*>((yyvsp[-2].ast_val))->value != dynamic_cast<RelExpAST*>((yyvsp[0].ast_val))->value;
            ast->op = Ne;
            (yyval.ast_val) = ast;
      }
#line 2529 "src/sysy.tab.cc"
    break;

  case 82: /* LAndExp: EqExp  */
#line 1037 "src/sysy.y"
      {
            auto ast = new LAndExpAST();
            ast->eqexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            EqExpAST* t = dynamic_cast<EqExpAST*>((yyvsp[0].ast_val));
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            (yyval.ast_val) = ast;
      }
#line 2547 "src/sysy.tab.cc"
    break;

  case 83: /* LAndExp: LAndExp AND EqExp  */
#line 1051 "src/sysy.y"
      {
            auto ast = new LAndExpAST();
            ast->landexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->eqexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->value = dynamic_cast<LAndExpAST*>((yyvsp[-2].ast_val))->value && dynamic_cast<EqExpAST*>((yyvsp[0].ast_val))->value;
            ast->op = And;
            (yyval.ast_val) = ast;
      }
#line 2560 "src/sysy.tab.cc"
    break;

  case 84: /* LOrExp: LAndExp  */
#line 1062 "src/sysy.y"
      {
            auto ast = new LOrExpAST();
            ast->landexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            LAndExpAST* t = dynamic_cast<LAndExpAST*>((yyvsp[0].ast_val));
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            (yyval.ast_val) = ast;
      }
#line 2578 "src/sysy.tab.cc"
    break;

  case 85: /* LOrExp: LOrExp OR LAndExp  */
#line 1076 "src/sysy.y"
      {
            auto ast = new LOrExpAST();
            ast->lorexp = unique_ptr<BaseAST>((yyvsp[-2].ast_val));
            ast->landexp = unique_ptr<BaseAST>((yyvsp[0].ast_val));
            ast->value = dynamic_cast<LOrExpAST*>((yyvsp[-2].ast_val))->value || dynamic_cast<LAndExpAST*>((yyvsp[0].ast_val))->value;
            ast->op = Or;
            (yyval.ast_val) = ast;
      }
#line 2591 "src/sysy.tab.cc"
    break;


#line 2595 "src/sysy.tab.cc"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (AST, YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, AST);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, AST);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (AST, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, AST);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, AST);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1085 "src/sysy.y"


/* void yyerror(const char *s) {
    printf("Error: %s, At line %d \n", s, yylineno);
    
    error_flag = 1;
} */
// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(std::unique_ptr<BaseAST> &ast , const char *s) {
  cerr << "error: " << s << endl;
  error_flag = 1;
}


int main(int argc, char** argv) {
      FILE *input_file;
      FILE *output_file;
      
      // Check if the correct number of command-line arguments are provided
      if(argc != 3) {
            fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
            return 1;
      }
      
      // Open the input file
      if(!(input_file = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
      }
      
      // Open the output file
      if(!(output_file = fopen("IR_file", "w"))) {
            //perror(argv[2]);
            fclose(input_file); // Close the input file before exiting
            return 1;
      }
      
      // Redirect yyin to the input file
      yyin = input_file;
      
      // Redirect yyout to the output file
      yyout = output_file;
      for(int i = 0; i < 109; i++){
                  table_parse[i] = create("-head", NULL, NULL);
                  table_dump[i] = create("-head", NULL, NULL);
            }

      std::unique_ptr<BaseAST> ast;
      auto ret = yyparse(ast);
                        
      ast->Print();
      ast->Dump();
      fclose(input_file);
      fclose(output_file);

      std::string inputFilename = "IR_file";
      std::string outputFilename = argv[2];

      // Read intermediate code from file
      auto intermediateCode = readIntermediateCode(inputFilename);

      // Translate to RISC-V assembly
      auto riscvCode = translateToRISC(intermediateCode);

      // Write RISC-V assembly to file
      writeRISCVCode(riscvCode, outputFilename);

      std::cout << "Translation completed. RISC-V assembly code written to " << outputFilename << std::endl;

      if (error_flag) {
            return 1; // Return non-zero value on error
      } else {
            return 0;
      }
}


// 计数器，用于生成临时变量名
int temp_count = 1;

// 生成临时变量名
std::string generateTemp() {
    return "t" + std::to_string(temp_count++);
}

int label_count = 1;

std::string new_label() {
    return "L" + std::to_string(label_count++);
}


