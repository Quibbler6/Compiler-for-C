#include "sysy.tab.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int yyparse();
extern int yylex();
extern FILE* yyin;
extern int error_flag;


// enum StmtKind {
//     STMT_EXPR,
//     STMT_IF,
//     STMT_WHILE,
//     STMT_RETURN,
// };

// struct Stmt {
//     enum StmtKind kind;
//     union {
//         struct Expr *expr;
//         struct IfStmt *if_stmt;
//         struct WhileStmt *while_stmt;
//         struct ReturnStmt *return_stmt;
//     };
// }; 

// struct Expr {
//     struct AddExp* addexp;
// };

// enum AddKind {
    
// };

// struct AddExp
// {
//     enum AddKind kind;
//     union
//     {
//       struct MulExp* mulexp;
//       struct 
//     };
    
// };


// struct IfStmt {
//     struct Expr* cond;
//     struct Stmt* then_branch;
//     struct Stmt* else_branch;
// };

// struct WhileStmt {
//     struct Expr* cond;
//     struct Stmt* body;
// };

// struct ReturnStmt {
//     struct Expr* value;
// };

// // 打印表达式的函数，需要你根据实际的Expr结构体来实现
// void print_expr(struct Expr *expr) {
//     // 这里添加具体的打印逻辑
// }

// // 打印if语句的函数
// void print_if_stmt(struct IfStmt *if_stmt) {
//     printf("if (");
//     print_expr(if_stmt->cond);
//     printf(")\n");
//     print_stmt(if_stmt->then_branch);
//     if (if_stmt->else_branch != NULL) {
//         printf("else\n");
//         print_stmt(if_stmt->else_branch);
//     }
// }

// // 打印while语句的函数
// void print_while_stmt(struct WhileStmt *while_stmt) {
//     printf("while (");
//     print_expr(while_stmt->cond);
//     printf(")\n");
//     print_stmt(while_stmt->body);
// }

// // 打印return语句的函数
// void print_return_stmt(struct ReturnStmt *return_stmt) {
//     printf("return ");
//     if (return_stmt->value != NULL) {
//         print_expr(return_stmt->value);
//     }
//     printf(";\n");
// }

// void print_stmt(struct Stmt *stmt) {
//     switch (stmt->kind) {
//         case STMT_EXPR:
//             print_expr(stmt->expr);
//             break;
//         case STMT_IF:
//             print_if_stmt(stmt->if_stmt);
//             break;
//         case STMT_WHILE:
//             print_while_stmt(stmt->while_stmt);
//             break;
//         case STMT_RETURN:
//             print_return_stmt(stmt->return_stmt);
//             break;
//     }
// }


int main(int argc, char** argv) {
    if(argc == 2){
        if(!(yyin = fopen(argv[1], "r"))){
            perror(argv[1]);
            return 1;
        }
    }
    yyparse();
    printf("error : %d", error_flag);
    if (error_flag) {
        return 1; // Return non-zero value on error
    } else {
        return 0;
    }
}
