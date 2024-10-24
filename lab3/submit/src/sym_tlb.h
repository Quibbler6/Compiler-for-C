#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
using namespace std;
// class BaseAST;
typedef enum {
    FUNCTION_INT,
    FUNCTION_VOID,
    VARIABLE,
    NUMBER,
    FUNC_PARAM
} InfoType;

class Param{
    public:
    bool is_lval;
    std::string ident;
    int value;
    vector<int> bracketlist;
    Param(bool is_lval_, std::string ident_, int value_, const std::vector<int>& bracketlist_)
    : is_lval(is_lval_), ident(ident_), value(value_), bracketlist(bracketlist_) {}
};

struct info {
    InfoType type;
    std::vector<int> var;
    std::vector<Param> fun;
    int value;
    int id;
    bool global;
};

typedef struct info *meta;

struct bucket {
    string key;
    meta info;
    shared_ptr<bucket> next;
};

