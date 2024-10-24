%code requires {
  #include <memory>
  #include <string>
}

%{

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

%}

%parse-param { std::unique_ptr<BaseAST> &AST }

%union{
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
};
%token <str_val> ID
%token <int_val> INT
%token ADD MUL SUB DIV MOD ASSIGN NOT LE GE LT GT EQ NE AND OR LBRACE RBRACE LPAREN RPAREN SEMICOLON LBRACKET RBRACKET COMMA
%token AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO IF INTTYPE LONG REGISTER RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID VOLATILE WHILE
%type <int_val> Number
%type <int_val> UnaryOp
%type <ast_val> CompUnit Decl VarDefList VarDef BRACKETList InitValList InitVal FuncDef FuncFParamsOpt FuncFParams FuncFParam ArrayOpt BlockItems BlockItem Stmt MatchedStmt OpenStmt NonIfStmt OptExp Exp Cond LVal PrimaryExp UnaryExp FuncRParamsOpt FuncRParams Arrays MulExp AddExp RelExp EqExp LAndExp LOrExp
%%
Start         : CompUnit
            {
                  auto root = make_unique<StartAST>();
                  root->comp = unique_ptr<BaseAST>($1);
                  AST = move(root);
                  //fprintf(yyout, "//Intermediate code generated.\n");
            };

CompUnit      : CompUnit Decl 
            {
                  auto comp_unit = new CompUnitAST();
                  comp_unit->kind = COMP_DEL;
                  comp_unit->comp = unique_ptr<BaseAST>($1);
                  comp_unit->decl = unique_ptr<BaseAST>($2);
                  $$ = comp_unit;
                  
            }
            | CompUnit FuncDef
            {
                  auto comp_unit = new CompUnitAST();
                  comp_unit->kind = COMP_FUNC_DEF;
                  comp_unit->comp = unique_ptr<BaseAST>($1);
                  comp_unit->func_def = unique_ptr<BaseAST>($2);
                  $$ = comp_unit;
            }
            | Decl 
            {
                  auto comp_unit = new CompUnitAST();
                  comp_unit->kind = DECL;
                  comp_unit->decl = unique_ptr<BaseAST>($1);
                  $$ = comp_unit;
            }
            | FuncDef
            {
                  auto comp_unit = new CompUnitAST();
                  comp_unit->kind = FUNC_DEF;
                  comp_unit->func_def = unique_ptr<BaseAST>($1);
                  $$ = comp_unit;
            }
            ;

Decl          :  INTTYPE VarDefList SEMICOLON
            {
                  auto ast = new DeclAST();
                  ast->var_type = "INT";
                  ast->vardeflist = unique_ptr<BaseAST>($2);
                  ast->is_global = 1;
                  $$ = ast;
            }
;

VarDefList    : VarDef
            {
                  auto ast = new VarDefListAST();
                  ast->is_single = 1;
                  ast->vardef = unique_ptr<BaseAST>($1);
                  $$ = ast;
            }
                | VarDefList COMMA VarDef
            {
                  auto ast = new VarDefListAST();
                  ast->is_single = 0;
                  ast->vardeflist = unique_ptr<BaseAST>($1);
                  ast->vardef = unique_ptr<BaseAST>($3);
                  $$ = ast;
            }
                ;

VarDef        : ID BRACKETList
            {
                  auto ast = new VarDefAST();
                  ast->is_init = 0;
                  ast->ident = *unique_ptr<string>($1);
                  ast->bracketlist = unique_ptr<BaseAST>($2);
                  $$ = ast;

            }
                | ID BRACKETList ASSIGN InitVal
            {
                  auto ast = new VarDefAST();
                  ast->is_init = 1;
                  ast->ident = *unique_ptr<string>($1);
                  ast->bracketlist = unique_ptr<BaseAST>($2);
                  ast->initval = unique_ptr<BaseAST>($4);
                  $$ = ast;
            }
                ;
BRACKETList     : 
            {
                  auto ast = new BRACKETListAST();
                  $$ = ast;
            }
                | BRACKETList LBRACKET INT RBRACKET
            {
                  auto ast = new BRACKETListAST();
                  int v = $3;
                  ast->bracketlist = dynamic_cast<BRACKETListAST*>($1)->bracketlist;
                  ast->bracketlist.push_back(v);
                  ast->is_arrays = 0;
                  $$ = ast;
            }


InitVal       : Exp
      {
            // auto ast = new InitValAST();
            // ast->is_exp = 1;
            // ast->exp = unique_ptr<BaseAST>($1);
            ExpAST * t = dynamic_cast<ExpAST *>($1);
            // ast->value = t->value;
            // $$ = ast;
            $$ = createValueNode(t->value, $1, $1);
            
      }
            | LBRACE InitValList RBRACE
      {
            // auto ast = new InitValAST();
            // ast->is_exp = 0;
            // ast->initvallist = unique_ptr<BaseAST>($2);
            // $$ = ast;
            InitValListAST * pt = dynamic_cast<InitValListAST *>($2);
            $$ = createChildNode(pt);
      }     
            | LBRACE RBRACE
      {
            $$ = createChildNode(NULL);
      }
                ;

InitValList   : InitVal{$$ = $1;}
                | InitValList COMMA InitVal
                {
                  InitValListAST * tail, * t = dynamic_cast<InitValListAST *>($1);
                  for(tail = t; tail->next; tail = tail->next);
                  InitValListAST * pt = dynamic_cast<InitValListAST *>($3);
                  tail->next = pt;
                  $$ = t;
                }
                ;

FuncDef: INTTYPE ID LPAREN FuncFParamsOpt RPAREN 
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
      LBRACE BlockItems RBRACE 
      {     
            auto ast = new FuncDefAST();
            ast->fun_type = "INT";
            ast->ident = *unique_ptr<string>($2);
            ast->params = unique_ptr<BaseAST>($4);
            ast->block = unique_ptr<BaseAST>($8);
            $$ = ast;

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
        | VOID ID LPAREN FuncFParamsOpt RPAREN 
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
        LBRACE BlockItems RBRACE
        {
            auto ast = new FuncDefAST();
            ast->fun_type = "VOID";
            ast->ident = *unique_ptr<string>($2);
            ast->params = unique_ptr<BaseAST>($4);
            ast->block = unique_ptr<BaseAST>($8);
            $$ = ast;

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
        ;


FuncFParamsOpt: /* empty */
      {
            auto ast = new FuncFParamsOptAST();
            ast->funparams = NULL;
            $$ = ast;
      }
              | FuncFParams
      {
            auto ast = new FuncFParamsOptAST();
            ast->funparams = unique_ptr<BaseAST>($1);
            $$ = ast;
      }
              ;

FuncFParams: FuncFParam
      {
            auto ast = new FuncFParamsAST();
            FuncParamAST * t = dynamic_cast<FuncParamAST*>($1);
            ast->funparams.push_back(*t);
            $$ = ast;
      }
           | FuncFParams COMMA FuncFParam
      {
            auto ast = new FuncFParamsAST();
            FuncFParamsAST * t = dynamic_cast<FuncFParamsAST*>($1);
            FuncParamAST * p = dynamic_cast<FuncParamAST*>($3);
            ast->funparams = t->funparams;
            ast->funparams.push_back(*p);
            $$ = ast;
      }
           ;

FuncFParam: INTTYPE ID ArrayOpt
      {
            auto ast = new FuncParamAST();
            ast->ident = *unique_ptr<string>($2);
            ast->is_lval = 1;
            if($3 != NULL){
                  BRACKETListAST * t = dynamic_cast<BRACKETListAST*>($3);
                  ast->bracketlist = t->bracketlist;
                  ast->bracketlist.insert(ast->bracketlist.begin(), 2003);
                  //std::cout<<"list size : " << ast->bracketlist.size() << endl;
            }
            $$ = ast;
      }
          ;

ArrayOpt: /* empty */
      {
            $$ = NULL;
      }
        | LBRACKET RBRACKET BRACKETList
      {
            $$ = $3;
      }
        ;


BlockItems: /* empty */
      {
            auto ast = new BlockItemsAST();
            ast->empty = 1;
            $$ = ast;
      }
          | BlockItems BlockItem
      {
            auto ast = new BlockItemsAST();
            ast->blockitems = unique_ptr<BaseAST>($1);
            ast->block = unique_ptr<BaseAST>($2);
            ast->empty = 0;
            $$ = ast;
      }
          ;

BlockItem: Decl
      {
            auto ast = new BlockItemAST();
            ast->is_decl = 1;
            ast->decl = unique_ptr<BaseAST>($1);
            DeclAST * t = dynamic_cast<DeclAST *>(ast->decl.get());
            t->is_global = 0;
            cout << "IS Global : "<< t->is_global <<endl;
            $$ = ast;
      }
         | Stmt
      {
            auto ast = new BlockItemAST();
            ast->is_decl = 0;
            ast->stmt = unique_ptr<BaseAST>($1);
            $$ = ast;
      }
         ;

Stmt: MatchedStmt
      {
            auto ast = new StmtAST();
            ast->is_open = 0;
            ast->matched = unique_ptr<BaseAST>($1);
            $$ = ast;
      } 
      | OpenStmt
      {
            auto ast = new StmtAST();
            ast->is_open = 1;
            ast->open = unique_ptr<BaseAST>($1);
            $$ = ast;
      };

MatchedStmt:
      IF LPAREN Cond RPAREN MatchedStmt ELSE MatchedStmt
      {
            auto ast = new MatchedStmtAST();
            ast->is_if = 1;
            ast->cond = unique_ptr<BaseAST>($3);
            ast->matched1 = unique_ptr<BaseAST>($5);
            ast->matched2 = unique_ptr<BaseAST>($7);
            $$ = ast;
      }
      | NonIfStmt
      {
            auto ast = new MatchedStmtAST();
            ast->is_if = 0;
            ast->noif = unique_ptr<BaseAST>($1);
            $$ = ast;
      }
    ;

OpenStmt:
      IF LPAREN Cond RPAREN Stmt
      {
            auto ast = new OpenStmtAST();
            ast->is_match = 0;
            ast->cond = unique_ptr<BaseAST>($3);
            ast->stmt = unique_ptr<BaseAST>($5);
            $$ = ast;
      }
    | IF LPAREN Cond RPAREN MatchedStmt ELSE OpenStmt
      {
            auto ast = new OpenStmtAST();
            ast->is_match = 1;
            ast->cond = unique_ptr<BaseAST>($3);
            ast->matched = unique_ptr<BaseAST>($5);
            ast->open = unique_ptr<BaseAST>($7);
            $$ = ast;
      }
    ;

NonIfStmt:
      LVal ASSIGN Exp SEMICOLON
      {
            auto ast = new NonIfStmtAST();
            ast->type = lv;
            ast->lval = unique_ptr<BaseAST>($1);
            ast->exp = unique_ptr<BaseAST>($3);
            $$ = ast;
      }
    | OptExp SEMICOLON
      {
            auto ast = new NonIfStmtAST();
            ast->type = opt;
            ast->exp = unique_ptr<BaseAST>($1);
            $$ = ast;
      }
    | LBRACE
    {
      //beginscope();
      } BlockItems RBRACE
      {
            //endscope();
            auto ast = new NonIfStmtAST();
            ast->type = lbr;
            ast->blockitems = unique_ptr<BaseAST>($3);
            $$ = ast;
      }
    | WHILE LPAREN Cond RPAREN Stmt
      {
            auto ast = new NonIfStmtAST();
            ast->type = whi;
            ast->cond = unique_ptr<BaseAST>($3);
            ast->stmt = unique_ptr<BaseAST>($5);
            $$ = ast;
      }
    | RETURN OptExp SEMICOLON
      {
            auto ast = new NonIfStmtAST();
            ast->type = ret;
            ast->exp = unique_ptr<BaseAST>($2);
            $$ = ast;
      }
    ;

OptExp: /* empty */
      {
            $$ = NULL;
      }
      | Exp
      {
            $$ = $1;
      }
      ;

Exp:  AddExp
      {
            auto ast = new ExpAST();
            ast->addexp = unique_ptr<BaseAST>($1);
            AddExpAST* t = dynamic_cast<AddExpAST*>($1);
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            $$ = ast;
            
      };

Cond: LOrExp
      {
            auto ast = new CondAST();
            ast->lorexp = unique_ptr<BaseAST>($1);
            $$ = ast;
      };

LVal: ID Arrays
      {
            auto ast = new FuncParamAST();
            ast->ident = *unique_ptr<string>($1);
            BRACKETListAST * t = dynamic_cast<BRACKETListAST*>($2);
            ast->bracketlist = t->bracketlist;
            ast->explist = t->explist;
            ast->is_arrays = t->is_arrays;
            $$ = ast;
      };

PrimaryExp: LPAREN Exp RPAREN
      {
            auto ast = new PrimaryExpAST();
            ast->type = EXP;
            ast->exp = unique_ptr<BaseAST>($2);
            ast->value = dynamic_cast<ExpAST*>($2)->value;
            ast->ident = dynamic_cast<ExpAST*>($2)->ident;
            ast->bracketlist = dynamic_cast<ExpAST*>($2)->bracketlist;
            $$ = ast;
      }
          | LVal
      {
            auto ast = new PrimaryExpAST();
            ast->type = Lval;
            FuncParamAST * t = dynamic_cast<FuncParamAST*>($1);
            ast->ident = t->ident;
            ast->bracketlist = t->bracketlist;
            ast->explist = t->explist;
            $$ = ast;
      }
          | Number
      {
            auto ast = new PrimaryExpAST();
            ast->type = Num;
            ast->value = $1;
            $$ = ast;
      }
          ;

Number: INT{$$ = $1;};

UnaryExp: PrimaryExp
      {
            auto ast = new UnaryExpAST();
            PrimaryExpAST* t = dynamic_cast<PrimaryExpAST*>($1);
            if(t->type == Lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            
            ast->primaryexp = unique_ptr<BaseAST>($1);
            ast->op = NONE;
            $$ = ast;
      }
        | ID LPAREN FuncRParamsOpt RPAREN
      {
            auto ast = new UnaryExpAST();
            auto fun = new FunCallAST();
            fun->ident = *unique_ptr<string>($1);
            fun->funparams = unique_ptr<BaseAST>($3);
            ast->funcall = unique_ptr<BaseAST>(fun);
            ast->op = FUNCALL;
            $$ = ast;
      }
        | UnaryOp UnaryExp
      {
            auto ast = new UnaryExpAST();
            switch ($1) {
            case Add:
                  ast->value = + dynamic_cast<UnaryExpAST*>($2)->value;
                  ast->op = Add;
                  break;
            case Sub:
                  ast->value = - dynamic_cast<UnaryExpAST*>($2)->value;
                  ast->op = Sub;
                  break;
            case Not:
                  ast->value = ! dynamic_cast<UnaryExpAST*>($2)->value;
                  ast->op = Not;
                  break;
            default : break;
      }
            std::string id = dynamic_cast<UnaryExpAST*>($2)->ident;
            ast->unaryexp = unique_ptr<BaseAST>($2);
            
            $$ = ast;
      }
        ;

UnaryOp: ADD{$$ = Add;}
       | SUB{$$ = Sub;}
       | NOT{$$ = Not;}
       ;

FuncRParamsOpt: /* empty */
      {
            auto ast = new FuncFParamsOptAST();
            ast->funparams = NULL;
            $$ = ast;
      }
              | FuncRParams
      {
            auto ast = new FuncFParamsOptAST();
            ast->funparams = unique_ptr<BaseAST>($1);
            FuncFParamsAST * t = dynamic_cast<FuncFParamsAST*>($1);
            //std::cout << "length : "<< t->funparams.size()<<endl;
            
            $$ = ast;
      }
              ;

FuncRParams: Exp
      {
            auto ast = new FuncFParamsAST();
            auto e = new FuncParamAST();
            ExpAST * t = dynamic_cast<ExpAST*>($1);
            if(t->is_lval){
                  e->is_lval = 1;
                  e->ident = t->ident;
                  e->bracketlist = t->bracketlist;
            }
            else{
                  e->is_lval = 0;
                  e->value = t->value;
            }
            e->exp = unique_ptr<BaseAST>($1);
            ast->funparams.push_back(*e);
            $$ = ast;
      }
           | FuncRParams COMMA Exp
      {
            auto ast = new FuncFParamsAST();
            FuncFParamsAST * t = dynamic_cast<FuncFParamsAST*>($1);
            ast->funparams = t->funparams;
            auto e = new FuncParamAST();
            ExpAST * p = dynamic_cast<ExpAST*>($3);
            
            if(p->is_lval){
                  e->is_lval = 1;
                  e->ident = p->ident;
                  e->bracketlist = p->bracketlist;
            }
            else{
                  e->is_lval = 0;
                  e->value = p->value;
            }
            e->exp = unique_ptr<BaseAST>($3);
            ast->funparams.push_back(*e);
            $$ = ast;
            
      }
      
           ;

Arrays: /* empty */
      {
            auto ast = new BRACKETListAST();
            $$ = ast;
      }
        | Arrays LBRACKET Exp RBRACKET
        {
            auto ast = new BRACKETListAST();
            //int v = dynamic_cast<ExpAST*>($3)->value;
            ast->bracketlist = dynamic_cast<BRACKETListAST*>($1)->bracketlist;
            ast->explist = dynamic_cast<BRACKETListAST*>($1)->explist;
            ast->explist.push_back($3);
            ast->is_arrays = 1;
            $$ = ast;
        }
        ;
        

MulExp: UnaryExp
      {
            auto ast = new MulExpAST();
            ast->unaryexp = unique_ptr<BaseAST>($1);
            UnaryExpAST* t = dynamic_cast<UnaryExpAST*>($1);
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            $$ = ast;
      }
      | MulExp MUL UnaryExp
      {
            auto ast = new MulExpAST();
            ast->mulexp = unique_ptr<BaseAST>($1);
            ast->unaryexp = unique_ptr<BaseAST>($3);
            ast->value = dynamic_cast<MulExpAST*>($1)->value * dynamic_cast<UnaryExpAST*>($3)->value;
            ast->op = Mul;
            $$ = ast;
      }
      | MulExp DIV UnaryExp
      {
            auto ast = new MulExpAST();
            ast->mulexp = unique_ptr<BaseAST>($1);
            ast->unaryexp = unique_ptr<BaseAST>($3);
            //ast->value = dynamic_cast<MulExpAST*>($1)->value / dynamic_cast<UnaryExpAST*>($3)->value;
            ast->op = Div;
            $$ = ast;
      }
      | MulExp MOD UnaryExp
      {
            auto ast = new MulExpAST();
            ast->mulexp = unique_ptr<BaseAST>($1);
            ast->unaryexp = unique_ptr<BaseAST>($3);
            //ast->value = dynamic_cast<MulExpAST*>($1)->value % dynamic_cast<UnaryExpAST*>($3)->value;
            ast->op = Mod;
            $$ = ast;
      }
      ;

AddExp: MulExp
      {
            auto ast = new AddExpAST();
            ast->mulexp = unique_ptr<BaseAST>($1);
            MulExpAST* t = dynamic_cast<MulExpAST*>($1);
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            $$ = ast;
      }
      | AddExp ADD MulExp
      {
            auto ast = new AddExpAST();
            ast->addexp = unique_ptr<BaseAST>($1);
            ast->mulexp = unique_ptr<BaseAST>($3);
            ast->value = dynamic_cast<AddExpAST*>($1)->value + dynamic_cast<MulExpAST*>($3)->value;
            ast->op = Add;
            $$ = ast;
      }
      | AddExp SUB MulExp
      {
            auto ast = new AddExpAST();
            ast->addexp = unique_ptr<BaseAST>($1);
            ast->mulexp = unique_ptr<BaseAST>($3);
            ast->value = dynamic_cast<AddExpAST*>($1)->value - dynamic_cast<MulExpAST*>($3)->value;
            ast->op = Sub;
            $$ = ast;
      }
      ;

RelExp: AddExp
      {
            auto ast = new RelExpAST();
            ast->addexp = unique_ptr<BaseAST>($1);
            AddExpAST* t = dynamic_cast<AddExpAST*>($1);
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            $$ = ast;
      }
      | RelExp LT AddExp
      {
            auto ast = new RelExpAST();
            ast->relexp = unique_ptr<BaseAST>($1);
            ast->addexp = unique_ptr<BaseAST>($3);
            ast->value = dynamic_cast<RelExpAST*>($1)->value < dynamic_cast<AddExpAST*>($3)->value;
            ast->op = Lt;
            $$ = ast;
      }
      | RelExp GT AddExp
      {
            auto ast = new RelExpAST();
            ast->relexp = unique_ptr<BaseAST>($1);
            ast->addexp = unique_ptr<BaseAST>($3);
            ast->value = dynamic_cast<RelExpAST*>($1)->value > dynamic_cast<AddExpAST*>($3)->value;
            ast->op = Gt;
            $$ = ast;
      }
      | RelExp LE AddExp
      {
            auto ast = new RelExpAST();
            ast->relexp = unique_ptr<BaseAST>($1);
            ast->addexp = unique_ptr<BaseAST>($3);
            ast->value = dynamic_cast<RelExpAST*>($1)->value <= dynamic_cast<AddExpAST*>($3)->value;
            ast->op = Le;
            $$ = ast;
      }
      | RelExp GE AddExp
      {
            auto ast = new RelExpAST();
            ast->relexp = unique_ptr<BaseAST>($1);
            ast->addexp = unique_ptr<BaseAST>($3);
            ast->value = dynamic_cast<RelExpAST*>($1)->value >= dynamic_cast<AddExpAST*>($3)->value;
            ast->op = Ge;
            $$ = ast;
      }
      ;

EqExp: RelExp
      {
            auto ast = new EqExpAST();
            ast->relexp = unique_ptr<BaseAST>($1);
            RelExpAST* t = dynamic_cast<RelExpAST*>($1);
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            $$ = ast;
      }
     | EqExp EQ RelExp
     {
            auto ast = new EqExpAST();
            ast->eqexp = unique_ptr<BaseAST>($1);
            ast->relexp = unique_ptr<BaseAST>($3);
            ast->value = dynamic_cast<EqExpAST*>($1)->value == dynamic_cast<RelExpAST*>($3)->value;
            ast->op = Eq;
            $$ = ast;
      }
     | EqExp NE RelExp
     {
            auto ast = new EqExpAST();
            ast->eqexp = unique_ptr<BaseAST>($1);
            ast->relexp = unique_ptr<BaseAST>($3);
            ast->value = dynamic_cast<EqExpAST*>($1)->value != dynamic_cast<RelExpAST*>($3)->value;
            ast->op = Ne;
            $$ = ast;
      }
     ;

LAndExp: EqExp
      {
            auto ast = new LAndExpAST();
            ast->eqexp = unique_ptr<BaseAST>($1);
            EqExpAST* t = dynamic_cast<EqExpAST*>($1);
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            $$ = ast;
      }
        | LAndExp AND EqExp
      {
            auto ast = new LAndExpAST();
            ast->landexp = unique_ptr<BaseAST>($1);
            ast->eqexp = unique_ptr<BaseAST>($3);
            ast->value = dynamic_cast<LAndExpAST*>($1)->value && dynamic_cast<EqExpAST*>($3)->value;
            ast->op = And;
            $$ = ast;
      }
        ;

LOrExp: LAndExp
      {
            auto ast = new LOrExpAST();
            ast->landexp = unique_ptr<BaseAST>($1);
            LAndExpAST* t = dynamic_cast<LAndExpAST*>($1);
            if(t->is_lval){
                  ast->is_lval = 1;
                  ast->ident = t->ident;
                  ast->bracketlist = t->bracketlist;
            }
            else ast->value = t->value;
            ast->op = NONE;
            $$ = ast;
      }
       | LOrExp OR LAndExp
      {
            auto ast = new LOrExpAST();
            ast->lorexp = unique_ptr<BaseAST>($1);
            ast->landexp = unique_ptr<BaseAST>($3);
            ast->value = dynamic_cast<LOrExpAST*>($1)->value || dynamic_cast<LAndExpAST*>($3)->value;
            ast->op = Or;
            $$ = ast;
      }
       ;
%%

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


