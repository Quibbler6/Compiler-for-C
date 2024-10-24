%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sysy.tab.hh"
int error_flag = 0;
char * yystr;
char * type1 = "int";
char * type2 = "void";
void yyerror(const char *s);
extern int yylex(void);
extern int yylineno;

struct YYSTYPE::ParamList;
struct YYSTYPE::FuncFParam;

typedef unsigned int uint;
typedef char* string;
typedef enum {
    INT_TYPE,
    VOID_TYPE
} BaseType;

typedef struct funinfo {
    BaseType return_type;
    struct YYSTYPE::ParamList list;
} funinfo;

typedef struct varinfo {
    BaseType var_type;
    int array_length;
    int array_size[10]; //array_size[i] meaning the size of var's ith dimension.
} varinfo;

typedef enum {
    FUNCTION_INT,
    FUNCTION_VOID,
    VARIABLE,
    NUMBER
} InfoType;

struct info {
    InfoType type;
    union {
        funinfo f;
        varinfo v;
    };
};

typedef struct info *meta;

meta init_meta(int is_var, BaseType type, int array_length, int *array_size, struct YYSTYPE::ParamList list) {
    meta m = (meta)malloc(sizeof(struct info));
    if (is_var) {
        m->type = VARIABLE;
        m->v.var_type = type;
        m->v.array_length = array_length;
        if (array_length > 0 && array_size != NULL) {
            memcpy(m->v.array_size, array_size, array_length * sizeof(int));
        }
    } else {
      if(type == INT_TYPE)
        {m->type = FUNCTION_INT;}
      else {m->type = FUNCTION_VOID;}
        m->f.return_type = type;
        m->f.list = list;
    }
    return m;
}

struct bucket {
      string key;
      meta info;
      struct bucket* next;
};
#define SIZE 109
struct bucket *table[SIZE];

unsigned int hash(char *str){
      unsigned int h = 0;
      char *s;
      for(s = str; *s; s++){
            h = h*65599 + *s;
      }
      return h;
}

struct bucket * create(string key, meta info, struct bucket *next){
      struct bucket * b = (struct bucket *)malloc(sizeof(struct bucket));
      b->key = key;
      b->info = info;
      b->next = next;
      return b;
}
void insert(string key, meta info){
      uint index = hash(key)%SIZE;
      struct bucket * b = create(key, info, table[index]->next);
      table[index]->next = b;
}
meta lookup(string key){
      uint index = hash(key)%SIZE;
      struct bucket* b;
      for(b = table[index]; b != NULL; b = b->next){
            if(0 == strcmp(b->key, key)){
                  return b->info;
            }
      }
      return NULL;
}
int check_redef(string key){
      uint index = hash(key)%SIZE;
      struct bucket* b;
      for(b = table[index]; b != NULL&&strcmp(b->key,"*"); b = b->next){
            if(0 == strcmp(b->key, key)){
                  return false;
            }
      }
      return true;
}
int fun_redef(string key){
      uint index = hash(key)%SIZE;
      struct bucket* b;
      for(b = table[index]; b != NULL; b = b->next){
            if(0 == strcmp(b->key, key)){
                  return false;
            }
      }
      return true;
}
void pop(string key){
      uint index = hash(key)%SIZE;
      struct bucket* temp = table[index]->next->next;
      free(table[index]->next);
      table[index]->next = temp;
}
void beginscope(){
      for(int i = 0; i < 109; i++){
        struct bucket * special = create("*", NULL, NULL);
        special->next = table[i]->next;
        table[i]->next = special;
    }
}
void endscope(){
      for(int i = 0; i < 109; i++){
            struct bucket * head;
            for(head = table[i]->next; head != NULL&&strcmp(head->key, "*"); ){
                  struct bucket* temp = head->next;
                  free(head);
                  head = temp;
                  table[i]->next = head;
            }
            struct bucket* temp = head->next;
            free(head);
            head = temp;
            table[i]->next = head;
      }
}
void print_meta(meta m) {
      if(m == NULL){
             printf("NULL\n");
             return;
      }
      if (m->type == FUNCTION_INT || m->type == FUNCTION_VOID) {
            printf("Type: FUNCTION\n");
            printf("Return Type: %d\n", m->f.return_type);
            printf("Parameter List:\n");
            printf("Parameter List Length: %d\n", m->f.list.length);
            for (int i = 0; i < m->f.list.length; i++) {
                  printf("  Parameter %d:\n", i + 1);
                  printf("    Length: %d\n", m->f.list.f[i].length);
                  printf("    Array Size: ");
                  for (int j = 0; j < m->f.list.f[i].length; j++) {
                  printf("%d ", m->f.list.f[i].array_size[j]);
                  }
                  printf("\n");
                  printf("    String: %s\n", m->f.list.f[i].key);
            }
      } else if (m->type == VARIABLE) {
            printf("Type: VARIABLE\n");
            printf("Variable Type: %d\n", m->v.var_type);
            if (m->v.array_length > 0) {
                  printf("Array Length: %d\n", m->v.array_length);
                  printf("Array Size: ");
                  for (int i = 0; i < m->v.array_length; i++) {
                  printf("%d ", m->v.array_size[i]);
                  }
                  printf("\n");
            }
      }
}
// 创建值节点
struct YYSTYPE::InitList * createValueNode(int value) {
    struct YYSTYPE::InitList * node = (struct YYSTYPE::InitList *)malloc(sizeof(struct YYSTYPE::InitList));
    if (node != NULL) {
        node->next = NULL;
        node->isValue = 1;
        node->data.value = value;
    }
    return node;
}

// 创建子列表节点
struct YYSTYPE::InitList * createChildNode(struct YYSTYPE::InitList * child) {
    struct YYSTYPE::InitList * node = (struct YYSTYPE::InitList *)malloc(sizeof(struct YYSTYPE::InitList));
    if (node != NULL) {
        node->next = NULL;
        node->isValue = 0;
        node->data.child = child;
    }
    return node;
}

// 释放节点及其后续节点的内存
void freeInitList(struct YYSTYPE::InitList * head) {
    while (head != NULL) {
        struct YYSTYPE::InitList * next = head->next;
        if (!head->isValue) {
            freeInitList(head->data.child);
        }
        free(head);
        head = next;
    }
}
void printInitList(struct YYSTYPE::InitList * head) {
      if(head == NULL) return;
    printf("{ ");
    while (head != NULL) {
        if (head->isValue) {
            printf("%d ", head->data.value);
        } else {
            printInitList(head->data.child);
        }
        head = head->next;
        if (head != NULL && head->next != NULL) {
            printf(", ");
        }
    }
    printf("}");
}
int add_value(int *len, int *array_size, int length) {
    int i;
    for (i = 0; i < length; ++i) {
        len[i] += 1; // 当前位加1

        // 如果当前位超过了该位的最大值
        if (len[i] >= array_size[i]) {
            len[i] = 0; // 当前位清零
        } else {
            return 1; // 正常递增返回1
        }
    }

    // 如果超出了最高位的最大值，返回0
    return 0;
}
int carry_value(int *len, int *array_size, int length) {
    int i;
    for (i = 0; i < length; ++i) {
        if (len[i] != 0 || i == length -1) {
            len[i]++;
            return 1;
            }
    }

    // 如果超出了最高位的最大值，返回0
    return 0;
}
// int check_initlist(struct YYSTYPE::InitList * root, int * array_size, int length, int * len){
//                         struct YYSTYPE::InitList* head = root;
//                         struct YYSTYPE::InitList* current = root->data.child;
//                         while(current){
//                               if(current->isValue){
//                                     if(add_value(len, array_size, length));
//                                     else return 0;
//                               }
//                               else 
//                               {
//                                     int t = carry_value(len, array_size, length);
//                                     if(t == 0) return 0;
//                               }
//                               current = current->next;
//                         }

//                   }
int zero_fill(struct YYSTYPE::InitList * root, int size, int &index, int * val, int * array_size, int length){
      struct YYSTYPE::InitList* head = root;
      struct YYSTYPE::InitList* current = root->data.child;
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
%}


%union
{
int number;
char *string;
struct BRACKETList{
      int length;
      int *array_size;
}list1;
struct FuncFParam{//Expr 
      int type;
      int length;
      int *array_size;
      char *key;
      int value;
}funparam;
struct ParamList{
      struct YYSTYPE::FuncFParam f[30];
      int length;
}paralist;
/* struct Expr{
      int value;
      int length;
      int *array_size;
      char *key;
}expr; */
struct Stmt{
      int Ret_type[30];
      int length;
} stmt;
// 节点类型
struct InitList {
    struct InitList* next;  // 指向下一个节点，值或子列表
    int isValue;            // 是否为值，0代表子列表，1代表值
    union {
        int value;          // 值节点的值
        struct InitList* child; // 子列表节点
    } data;
} * initlist;
}

%token <string> ID 
%token <number> INT
%token ADD MUL SUB DIV MOD ASSIGN NOT LE GE LT GT EQ NE AND OR LBRACE RBRACE LPAREN RPAREN SEMICOLON LBRACKET RBRACKET COMMA
%token AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO IF INTTYPE LONG REGISTER RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID VOLATILE WHILE
%type <number> Number
%type <list1> BRACKETList Arrays ArrayOpt
%type <funparam> FuncFParam LVal Exp EqExp LAndExp LOrExp AddExp MulExp PrimaryExp UnaryExp OptExp RelExp Cond
%type <paralist> FuncFParams FuncFParamsOpt FuncRParams FuncRParamsOpt
%type <stmt> NonIfStmt Stmt MatchedStmt OpenStmt BlockItem BlockItems
%type <initlist> InitVal InitValList 
%%

CompUnit      : CompUnit Decl | CompUnit FuncDef | Decl | FuncDef;

Decl          : VarDecl;
VarDecl       : INTTYPE VarDefList SEMICOLON;

VarDefList    : VarDef
                | VarDefList COMMA VarDef
                ;

VarDef        : ID  BRACKETList 
                  {
                  //printf("length : %d\n", $2.length);
                  struct YYSTYPE::ParamList empty;
                  empty.length = 0;
                  //empty.f = (struct YYSTYPE::FuncFParam *)malloc(0);
                  meta m = init_meta(1, INT_TYPE, $2.length, $2.array_size, empty);
                  if(check_redef($1) == false){
                        yyerror("var redefine error");
                  }
                  else insert($1, m);
                  print_meta(m);
                  }
                | ID  BRACKETList ASSIGN InitVal
                {
                  struct YYSTYPE::ParamList empty;
                  empty.length = 0;
                  //empty.f = (struct YYSTYPE::FuncFParam *)malloc(0);
                  meta m = init_meta(1, INT_TYPE, $2.length, $2.array_size, empty);
                  if(check_redef($1) == false){
                        yyerror("var redefine error");
                  }
                  else insert($1, m);
                  if($4->isValue == 0){
                        printInitList($4);
                        printf("\n");
                        int len[30] = {0};
                        int size = 1;
                        for(int i = 0; i < $2.length; i++){
                              size *= $2.array_size[i];
                        }
                        int * val = (int *)malloc(sizeof(int) * 2*size);
                        int index = 0;
                        if(zero_fill($4, size, index, val, $2.array_size, $2.length)) 
                              {
                              for(int i = 0; i < size; i++)
                                    printf("%d, ", val[i]);
                              printf("\n");
                              yyerror("init_list not match");
                        }
                        freeInitList($4);
                  }
                  
                  }
                ;
BRACKETList : {$$.length = 0;$$.array_size = (int *)malloc(0);}
            | LBRACKET INT RBRACKET BRACKETList {
                  $4.length++;
                  $4.array_size = (int *)realloc($4.array_size, $4.length * sizeof(int));
                  if ($4.array_size == NULL) {
                      // 处理内存分配失败的情况
                      fprintf(stderr, "Memory allocation failed\n");
                      exit(EXIT_FAILURE);
                  }
                  $4.array_size[$4.length - 1] = $2;
                  //printf("length: %d, value: %d\n", $4.length, $2);
                  $$ = $4;
              }

InitVal       : Exp{
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) {}
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = createValueNode($1.value);
            
      }
                  
                | LBRACE InitValList RBRACE
                {
                  $$ = createChildNode($2);
                  $$->isValue = 0;
                }
                | LBRACE RBRACE
                {
                  $$ = createChildNode(NULL);
                  $$->isValue = 0;
                }
                ;
InitValList   : InitVal{$$ = $1;}
                | InitValList COMMA InitVal
                {
                  $$ = $1;
                  struct YYSTYPE::InitList * tail;
                  for(tail = $$; tail->next; tail = tail->next); 
                  tail->next = $3;
                }
                ;

FuncDef: INTTYPE ID LPAREN FuncFParamsOpt RPAREN 
      {
            meta m = init_meta(0,INT_TYPE, 0, NULL, $4);
            if(fun_redef($2) == false){
                  yyerror("fun redefine error");
            }
            else insert($2, m);

            beginscope();
            struct YYSTYPE::ParamList plist = $4;
            for(int i = 0; i < plist.length; i++){
                  struct YYSTYPE::ParamList empty;
                  empty.length = 0;
                  meta m = init_meta(1, INT_TYPE, plist.f[i].length ,plist.f[i].array_size, empty);
                  if(check_redef(plist.f[i].key) == false){
                        yyerror("param redefine error");
                  }
                  insert((plist.f[i].key), m);
            }
            
      }
      LBRACE BlockItems RBRACE
      {
            endscope();

            if($8.length == 0) yyerror("return type mismatched");
            else{
                  printf("num of ret is %d\n", $8.length);
                  for(int i = 0; i < $8.length; i++){
                        if($8.Ret_type[i] != INT_TYPE){
                        printf("RET_[%d] : %d\n", i, $8.Ret_type[i]);
                        yyerror("return type mismatched");
                        }
                  }
            }
            //if($8.Ret_type != INT_TYPE) yyerror("return type mismatched");
            
      }
        | VOID ID LPAREN FuncFParamsOpt RPAREN 
        {
            meta m = init_meta(0,VOID_TYPE, 0, NULL, $4);
            if(fun_redef($2) == false){
                  yyerror("fun redefine error");
            }
            else insert($2, m);
            
            beginscope();
            struct YYSTYPE::ParamList plist = $4;
            for(int i = 0; i < plist.length; i++){
                  struct YYSTYPE::ParamList empty;
                  empty.length = 0;
                  meta m = init_meta(1, INT_TYPE, plist.f[i].length ,plist.f[i].array_size, empty);
                  if(check_redef(plist.f[i].key) == false){
                        yyerror("param redefine error");
                  }
                  insert((plist.f[i].key), m);
            }
            
      }
        LBRACE BlockItems RBRACE
      {
            endscope();

            if($8.length == 0) yyerror("return type mismatched");
            else{
                  printf("num of ret is %d\n", $8.length);
                  for(int i = 0; i < $8.length; i++){
                        if($8.Ret_type[i] != VOID_TYPE){
                        printf("RET_[%d] : %d\n", i, $8.Ret_type[i]);
                        yyerror("return type mismatched");
                        }
                  }
            }
            //if($8.Ret_type != INT_TYPE) yyerror("return type mismatched");
      }
        ;


FuncFParamsOpt: /* empty */{$$.length = 0;/*$$.f = (struct YYSTYPE::FuncFParam *)malloc(0);*/}
              | FuncFParams{$$ = $1;printf("!! param length : %d\n", $1.length);}
              ;

FuncFParams: FuncFParam {
                        $$.length++;
                        //$$.f = (struct YYSTYPE::FuncFParam *)realloc($$.f, $$.length * sizeof(struct YYSTYPE::FuncFParam));
                        $$.f[0] = $1;
                        }
           | FuncFParam COMMA FuncFParams
           {
                  $3.length++;
                  //printf("!! param length : %d\n", $$.length);
                  // $$.f = (struct YYSTYPE::FuncFParam *)realloc($$.f, $$.length * sizeof(struct YYSTYPE::FuncFParam));
                  // if ($$.f == NULL) {
                  //     // 处理内存分配失败的情况
                  //     fprintf(stderr, "Memory allocation failed\n");
                  //     exit(EXIT_FAILURE);
                  // }
                  $3.f[$3.length - 1] = $1;
                  $$ = $3;
           }
           ;

FuncFParam: INTTYPE ID ArrayOpt
            {
                  $$.length = $3.length;
                  $$.key = $2;
                  $$.array_size = (int *)malloc($3.length * sizeof(int));
                  memcpy($$.array_size, $3.array_size, $3.length * sizeof(int));
            }
          ;

ArrayOpt: /* empty */{$$.length = 0;$$.array_size = (int *)malloc(0);}
        | LBRACKET RBRACKET BRACKETList
        {
            $3.length++;
            $3.array_size = (int *)realloc($3.array_size, $3.length * sizeof(int));
            if ($3.array_size == NULL) {
                  // 处理内存分配失败的情况
                  fprintf(stderr, "Memory allocation failed\n");
                  exit(EXIT_FAILURE);
            }
            $3.array_size[$3.length-1] = 0x7fff;
            $$ = $3;
        }
        ;


BlockItems: /* empty */{$$.length = 0;}
          | BlockItems BlockItem
          {
            
            for(int i = 0; i < $2.length; i++){
                  $1.Ret_type[i + $1.length] = $2.Ret_type[i];
            }
            $1.length += $2.length; 
            $$ = $1;
          }
          ;

BlockItem: Decl
         | Stmt
         {
            $$ = $1;
      }
         ;

Stmt: MatchedStmt
      {
            $$ = $1;
      }

      | OpenStmt 
      {
            $$ = $1;
      }
      
      ;

MatchedStmt:
      IF LPAREN Cond RPAREN MatchedStmt ELSE MatchedStmt 
      {     
            printf("%d, %d\n", $5.length, $7.length);
            for(int i = 0; i <  $5.length; i++){
                  $$.Ret_type[i] = $5.Ret_type[i];
            }
            $$.length += $5.length; 
            

            for(int i = 0; i <  $7.length; i++){
                  $$.Ret_type[i + $$.length] = $7.Ret_type[i];
            }
            $$.length += $7.length; 
            printf("ret[%d] : %d\n", $$.length - 1, $$.Ret_type[0]);
      }
    | NonIfStmt
      {
            printf("%d\n", $1.length);
            for(int i = 0; i <  $1.length; i++){
                  $$.Ret_type[i] = $1.Ret_type[i];
            }
            $$.length = $1.length; 
      }
    ;

OpenStmt:
      IF LPAREN Cond RPAREN Stmt
      {
            $$ = $5;
      }
    | IF LPAREN Cond RPAREN MatchedStmt ELSE OpenStmt
    {
            $$ = $5;

            for(int i = 0; i <  $7.length; i++){
                  $$.Ret_type[i + $$.length] = $7.Ret_type[i];
            }
            $$.length += $7.length; 
      }
    ;

NonIfStmt:
      LVal ASSIGN Exp SEMICOLON 
      {
            meta m = lookup($1.key);
            if(m == NULL) yyerror("var nonedef error");
            else{
                  if(m->type != VARIABLE) yyerror("var nonedef error");
                  else if(m->type == VARIABLE){
                        if(m->v.array_length == $1.length){
                              for(int i = 0; i < $1.length; i++){
                                    if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                          yyerror("beyond array range");
                              }
                        }
                        else{
                              yyerror("array demensions not match");
                        }
                  }
            }
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
      }
    | OptExp SEMICOLON
    | LBRACE {beginscope();} BlockItems RBRACE {endscope();$$ = $3;}
    | WHILE LPAREN Cond RPAREN Stmt
    | RETURN OptExp SEMICOLON{$$.length++; $$.Ret_type[$$.length - 1] = $2.type; printf("RET_[%d] : %d\n", $$.length - 1, $$.Ret_type[0]);}
    ;

OptExp: /* empty */{$$.type = VOID_TYPE;}
      | Exp {
      if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;
            $$.type = INT_TYPE;
      }
      ;

Exp:  AddExp{$$ = $1;};

Cond: LOrExp{
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;
      };

LVal: ID Arrays  
      {
            meta m = lookup($1);
            if(m == NULL) yyerror("var nonedef error");
            // else{
            //       if(m->type == FUNCTION_INT) yyerror("var nonedef error");
            //       else if(m->type == VARIABLE){
            //             if(m->v.array_length == $2.length){
            //                   for(int i = 0; i < $2.length; i++){
            //                         if($2.array_size[i] >= m->v.array_size[i] || $2.array_size[i] < 0)
            //                               yyerror("beyond array range");
            //                   }
            //             }
            //             else{
            //                   yyerror("array demensions not match");
            //             }
            //       }
            // }
            $$.type = VARIABLE;
            $$.key = $1;
            $$.length = $2.length;
            $$.array_size = (int *)malloc($2.length * sizeof(int));
            memcpy($$.array_size, $2.array_size, $2.length * sizeof(int));
      };

PrimaryExp: LPAREN Exp RPAREN{$$ = $2;}
          | LVal{$$ = $1;}
          | Number{$$.value = $1; $$.length = 0;$$.type = NUMBER;}
          ;

Number: INT{$$ = $1;};

UnaryExp: PrimaryExp{$$ = $1;}
        | ID LPAREN FuncRParamsOpt RPAREN
        {
            meta m = lookup($1);
            if(m == NULL) yyerror("fun nonedef error");
            else{
                  if(m->type == VARIABLE) yyerror("fun nonedef error");
                  else if(m->type == FUNCTION_INT || m->type == FUNCTION_VOID){
                        
                         if(m->f.list.length == $3.length){
                                for(int i = 0; i < $3.length; i++){
                                    //printf("$3.length : %d\n",$3.f[i].type);
                                    if($3.f[i].type == VARIABLE){
                                          meta v = lookup($3.f[i].key);
                                          if(v == NULL) yyerror("var nonedef error");
                                          if(m->f.list.f[i].length > 0){
                                                if(m->f.list.f[i].length == v->v.array_length && $3.f[i].length == 0){}
                                                else {
                                                      yyerror("params array dementions not match");
                                                }
                                          }
                                          else{
                                                if(v->v.array_length == $3.f[i].length){ 
                                                      for(int j = 0; j < $3.f[i].length; j++)
                                                      {     
                                                            if(v->v.array_size[j] < $3.f[i].array_size[j])
                                                                  yyerror("beyond array range");
                                                      }
                                                }
                                                else{ 
                                                      
                                                      yyerror("var array dementions not match");
                                                }
                                          }
                                    }
                                    else{
                                          if($3.f[i].type == NUMBER){
                                                //printf("NUMBER\n");
                                                if(m->f.list.f[i].length == 0){}
                                                else{
                                                      yyerror("params array dementions not match");
                                                }
                                          }
                                          if($3.f[i].type == FUNCTION_INT){
                                                //printf("FUNCTION\n");
                                                if(m->f.list.f[i].length == 0){}
                                                else{
                                                      yyerror("params array dementions not match");
                                                }
                                          }
                                          else if($3.f[i].type == FUNCTION_VOID) yyerror("func type not match");
                                    }
                                }
                         }
                         else{
                              yyerror("func params number not match");
                         }
                         if(m->type == FUNCTION_INT) $$.type = FUNCTION_INT;
                         else if(m->type == FUNCTION_VOID) {$$.type = FUNCTION_VOID;}
                  }
                  // else if(m->type == FUNCTION_VOID && m->f.return_type == VOID_TYPE) yyerror("func return type error");
                  $$.key = $1;
                  $$.length = 0;
                  //$$.type = m->type;
      }
        }
        | UnaryOp UnaryExp 
        {
            if($2.type != NUMBER){
                  meta m = lookup($2.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $2.length){
                                    for(int i = 0; i < $2.length; i++){
                                          if($2.array_size[i] >= m->v.array_size[i] || $2.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $2;
            $$.type = NUMBER;
            $$.length = 0;
      }
        ;

UnaryOp: ADD
       | SUB
       | NOT
       ;


FuncRParamsOpt: /* empty */{$$.length = 0;/*$$.f = (struct YYSTYPE::FuncFParam *)malloc(0);*/}
              | FuncRParams{$$ = $1;printf("!! param length : %d\n", $1.length);}
              ;

FuncRParams: Exp
            {
            $$.length = 1;
            $$.f[0] = $1;
            }
           | Exp COMMA FuncRParams
           {
                  $3.length++;
                  //printf("!! param length : %d\n", $$.length);
                  // $$.f = (struct YYSTYPE::FuncFParam *)realloc($$.f, $$.length * sizeof(struct YYSTYPE::FuncFParam));
                  // if ($$.f == NULL) {
                  //     // 处理内存分配失败的情况
                  //     fprintf(stderr, "Memory allocation failed\n");
                  //     exit(EXIT_FAILURE);
                  // }
                  $3.f[$3.length - 1] = $1;
                  $$ = $3;
           }
           ;

Arrays: /* empty */{$$.length = 0;$$.array_size = (int *)malloc(0);}
        | LBRACKET Exp RBRACKET Arrays
        {
            if($2.type != NUMBER){
                  meta m = lookup($2.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $2.length){
                                    for(int i = 0; i < $2.length; i++){
                                          if($2.array_size[i] >= m->v.array_size[i] || $2.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $4.length++;
            $4.array_size = (int *)realloc($4.array_size, $4.length * sizeof(int));
            if ($4.array_size == NULL) {
                  // 处理内存分配失败的情况
                  fprintf(stderr, "Memory allocation failed\n");
                  exit(EXIT_FAILURE);
            }
            if($2.length == 0)
                   $4.array_size[$4.length - 1] = $2.value;
            //printf("length: %d, value: %d\n", $4.length, $2);
            $$ = $4;
        }
        ;

MulExp: UnaryExp
      {
            $$ = $1;
      }

      | MulExp MUL UnaryExp
      {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
      | MulExp DIV UnaryExp
      {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
      | MulExp MOD UnaryExp
      {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
      ;

AddExp: MulExp{$$ = $1;}
      | AddExp ADD MulExp
      {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
      | AddExp SUB MulExp
      {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
      ;

RelExp: AddExp{$$ = $1;}
      | RelExp LT AddExp
      {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
      | RelExp GT AddExp
      {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
      | RelExp LE AddExp
      {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
      | RelExp GE AddExp
      {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
      ;

EqExp: RelExp{$$ = $1;}
     | EqExp EQ RelExp
     {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
     | EqExp NE RelExp
     {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
     ;

LAndExp: EqExp{$$ = $1;}
        | LAndExp AND EqExp
        {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
        ;

LOrExp: LAndExp{$$ = $1;}
       | LOrExp OR LAndExp
       {
            if($3.type != NUMBER){
                  meta m = lookup($3.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $3.length){
                                    for(int i = 0; i < $3.length; i++){
                                          if($3.array_size[i] >= m->v.array_size[i] || $3.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            if($1.type != NUMBER){
                  meta m = lookup($1.key);
                  if(m == NULL) yyerror("var nonedef error");
                  else{
                        if(m->type == FUNCTION_INT) ;
                        else if(m->type == FUNCTION_VOID) yyerror("func type not match");
                        else if(m->type == VARIABLE){
                              if(m->v.array_length == $1.length){
                                    for(int i = 0; i < $1.length; i++){
                                          if($1.array_size[i] >= m->v.array_size[i] || $1.array_size[i] < 0)
                                                yyerror("beyond array range");
                                    }
                              }
                              else{
                                    yyerror("array demensions not match");
                              }
                        }
                  }
            }
            $$ = $1;$$.type = NUMBER;
      }
       ;
%%

void yyerror(const char *s) {
    printf("Error: %s, At line %d \n", s, yylineno);
    
    error_flag = 1;
}

extern FILE* yyin;
//extern struct bucket;

int main(int argc, char** argv) {
      if(argc == 2){
            if(!(yyin = fopen(argv[1], "r"))){
                  perror(argv[1]);
                  return 1;
            }
      }
      for(int i = 0; i < 109; i++){
            table[i] = create("-head", NULL, NULL);
      }

      struct YYSTYPE::ParamList read;
      read.length = 0;
      read.f[0].length = 0;
      read.f[0].array_size  = NULL;
      read.f[0].key = NULL;
      //empty.f = (struct YYSTYPE::FuncFParam *)malloc(0);
      meta m = init_meta(0, INT_TYPE, 0, NULL, read);
      insert("read", m);
      print_meta(m);

      struct YYSTYPE::ParamList write;
      write.length = 1;
      write.f[0].length = 0;
      write.f[0].array_size  = NULL;
      write.f[0].key = NULL;

      //empty.f = (struct YYSTYPE::FuncFParam *)malloc(0);
      meta t = init_meta(0, INT_TYPE, 0, NULL, write);
      insert("write", t);
      print_meta(t);

      yyparse();
      printf("error : %d\n", error_flag);
      if (error_flag) {
            return 1; // Return non-zero value on error
      } else {
            return 0;
      }
}