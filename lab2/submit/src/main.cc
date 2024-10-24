// #include "sysy.tab.hh"
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// extern int yyparse();
// extern int yylex();
// extern FILE* yyin;
// extern int error_flag;
// extern struct bucket **table;
// extern struct bucket * create(string key, void *binding, struct bucket *next);
// //extern struct bucket;

// int main(int argc, char** argv) {
//     if(argc == 2){
//         if(!(yyin = fopen(argv[1], "r"))){
//             perror(argv[1]);
//             return 1;
//         }
//     }
//     for(int i = 0; i < 109; i++){
//         table[i] = create("-head", NULL, NULL);
//     }
//     yyparse();
//     printf("error : %d", error_flag);
//     if (error_flag) {
//         return 1; // Return non-zero value on error
//     } else {
//         return 0;
//     }
// }
