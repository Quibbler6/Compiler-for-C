#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "sym_tlb.h"
using namespace std;
extern FILE *yyout;
extern int temp_count;
extern std::string generateTemp();
extern int label_count;
extern std::string new_label();
extern meta lookup(const string& key, shared_ptr<bucket> table_param[]);
extern unsigned int _hash_(const string& str);
extern shared_ptr<bucket> create(const string& key, meta info, shared_ptr<bucket> next);
extern void insert(const string& key, meta info, shared_ptr<bucket> table_param[]);
extern shared_ptr<bucket> table_dump[];
extern void beginscope(shared_ptr<bucket> table[]);
extern void endscope(shared_ptr<bucket> table[]);
extern meta init_meta(InfoType type, std::vector<int> var, std::vector<Param> fun, int value);
extern int check_redef(const string& key, shared_ptr<bucket> table[]);
extern int fun_redef(const string& key, shared_ptr<bucket> table[]);
extern int error_flag;

class BaseAST {
    public:
    virtual ~BaseAST() = default;
    virtual void Print() const = 0;
    virtual void Dump() const = 0;
    virtual string translate_exp(string place) const = 0;
    virtual string translate_cond(string l1, string l2) const = 0;
};

class StartAST : public BaseAST{
    public:
    std::unique_ptr<BaseAST> comp;
    void Print() const override {
        std::cout << "Start : \n";
        comp->Print();
        std::cout << endl;
    }
    void Dump() const override {
        comp->Dump();
    }
    string translate_exp (string place)const override{return place;}
    string translate_cond(string l1, string l2)const override{return l1;}
};
enum CompKind {
    COMP_DEL,
    COMP_FUNC_DEF,
    FUNC_DEF,
    DECL,
};
// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
    public:
    enum CompKind kind;
    // 用智能指针管理对象
        std::unique_ptr<BaseAST> comp;
        std::unique_ptr<BaseAST> decl;
        std::unique_ptr<BaseAST> func_def;
    void Print() const override {
        //std::cout << "CompUnitAST : \n";
        switch (this->kind) {
        case COMP_DEL:
            this->comp->Print();
            this->decl->Print();
            break;
        case COMP_FUNC_DEF:
            this->comp->Print();
            this->func_def->Print();
            break;
        case FUNC_DEF:
            this->func_def->Print();
            break;
        case DECL:
            this->decl->Print();
            break;
        default : break;
    }
    //std::cout << "\n";
  }
  void Dump() const override {
        //std::cout << "CompUnitAST : \n";
        switch (this->kind) {
        case COMP_DEL:
            this->comp->Dump();
            this->decl->Dump();
            break;
        case COMP_FUNC_DEF:
            this->comp->Dump();
            this->func_def->Dump();
            break;
        case FUNC_DEF:
            this->func_def->Dump();
            break;
        case DECL:
            this->decl->Dump();
            break;
        default : break;
    }
    //std::cout << "\n";
  }
  string translate_exp (string place)const override{return place;}
  string translate_cond(string l1, string l2)const override{return l1;}
};

class BRACKETListAST : public BaseAST{
    public:
    bool is_arrays;
    vector<int> bracketlist;
    vector<BaseAST *> explist;
    void Print() const override {
        for (int i = 0; i < this->bracketlist.size(); i++) {
        cout << "["<< bracketlist[i] << "]";
    }
    }
    void Dump() const override {
        
    }
    string translate_exp (string place)const override{return place;}
    string translate_cond(string l1, string l2)const override{return l1;}
};


class FuncParamAST : public BaseAST {
    public:
    bool is_lval;
    bool is_arrays; //variable being used as exp or lval
    std::string ident;
    int value;
    vector<int> bracketlist;
    std::shared_ptr<BaseAST> exp;
    vector<BaseAST *> explist;
    void Print() const override {
        std::cout << ident;
        for(int i = 0; i < bracketlist.size(); i++){
            cout << "["<< bracketlist[i] << "]";
        }
    }
    void Dump() const override {
        
    }
    string translate_exp (string place)const override{return place;}
    FuncParamAST() = default;
    FuncParamAST(const FuncParamAST& other)
        : BaseAST(other),  // 复制基类成员
          is_lval(other.is_lval),
          ident(other.ident),
          value(other.value),
          bracketlist(other.bracketlist),
          explist(other.explist)
    {
        if (other.exp) {
            exp = std::move(other.exp); // 使用移动构造函数转移所有权
        } else {
            exp = nullptr;
        }
    
    }
    string translate_cond(string l1, string l2)const override{return l1;}
};

class FuncFParamsOptAST : public BaseAST{
    public:
    std::unique_ptr<BaseAST> funparams;
    void Print() const override {
        if(funparams != NULL){
            funparams->Print();
        }
    }
    void Dump() const override {
        if(funparams != NULL){
            funparams->Dump();
        }
    }
    string translate_exp (string place)const override{
        string code;
        if(funparams != NULL){
            code = funparams->translate_exp(place);
        }
        return code;
    }
    string translate_cond(string l1, string l2)const override{return l1;}
};

class FuncFParamsAST : public BaseAST{
    public:
    std::vector<FuncParamAST> funparams;
    void Print() const override {
        for(int i = 0; i < funparams.size(); i++){
            if(funparams[i].is_lval){
                std::cout << " " << funparams[i].ident << " : ";
                for (int j = 0; j < funparams[i].bracketlist.size(); j++) {
                    cout << "["<< funparams[i].bracketlist[j] << "]";
                }
            }
            else std::cout << " " << funparams[i].value << ", ";
        }
    }
    void Dump() const override {

    }
    string translate_exp (string place)const override{
        string code, t1 ,id;
        meta m;
        if(place == "ARG"){
        for(int i = 0; i < funparams.size(); i++){
            // if(funparams[i].is_lval && ){
            //     string id = funparams[i].ident;
            //     meta m = lookup(id);
            //     code += "ARG t" + to_string(m->id) + "\n";
            //     //fprintf(yyout, "ARG t%d\n", m->id);
            //     // for (int j = 0; j < funparams[i].bracketlist.size(); j++) {
            //     //     cout << "["<< funparams[i].bracketlist[j] << "]";
            //     // }
            // }
            // else{
                t1 = generateTemp();
                code += funparams[i].exp->translate_exp(t1);
                code += "ARG " + t1 + "\n";
            //}
        }
    }
        else if(place == "PARAM"){
        for(int i = 0; i < funparams.size(); i++){
            if(funparams[i].is_lval){
                id = funparams[i].ident;
                code += "PARAM " + id + "\n";
            }
        }
        for(int i = 0; i < funparams.size(); i++){
            if(funparams[i].is_lval){
                id = funparams[i].ident;
                m = lookup(id, table_dump);
                if(m == NULL){
                    cout << "FuncFParams not find param :" << id << endl;
                    exit(0);
                }
                m->id = temp_count;
                t1 = generateTemp();
                code += t1 + " = " + id + "\n";
            }
        }
    }
    return code;
    }
    string translate_cond(string l1, string l2)const override{return l1;}
};


// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
    public:
    std::string fun_type;
    std::string ident;
    std::unique_ptr<BaseAST> params;
    std::unique_ptr<BaseAST> block;

    void Print() const override {
        std::cout << "FuncDefAST : ";
        std::cout << fun_type ;
        std::cout << ": " << ident << "(";
        params->Print();
        std::cout << ")\n";
        block->Print();
        std::cout << endl;
    }
    void Dump() const override {
        string code;
        meta m;
        FuncFParamsOptAST * pt = dynamic_cast<FuncFParamsOptAST*>(params.get());
        FuncFParamsAST * pt1 = dynamic_cast<FuncFParamsAST*>(pt->funparams.get());
        std::vector<Param> fun;
        if(pt1!=NULL){
                for (const auto& param : pt1->funparams) {
                    Param elm = Param(param.is_lval, param.ident, param.value, param.bracketlist);
                    fun.push_back(elm);
                }
            }
        if(fun_type == "INT"){
            m = init_meta(FUNCTION_INT, {}, fun, 0);
        }
        else m = init_meta(FUNCTION_VOID, {}, fun, 0);
        if(fun_redef(ident, table_dump) == false){
                  error_flag = 1;
                  cout << "ERROR : func redef error" << endl;
            }
        else insert(ident, m, table_dump);

        beginscope(table_dump);
        if(pt1!=NULL){
                for (const auto& param : pt1->funparams) {
                    vector<int> var = param.bracketlist;
                    meta n = init_meta(VARIABLE, var, {}, 0);
                    if(check_redef(param.ident, table_dump) == false){
                    error_flag = 1;
                    cout << "ERROR : param redefine error" << endl;
                }
                else insert(param.ident, n, table_dump);
                }
            }

        fprintf(yyout, "FUNCTION %s:\n", ident.c_str());
        code += params->translate_exp("PARAM");
        fprintf(yyout, "%s", code.c_str());
        block->Dump();
        endscope(table_dump);
    }
    string translate_exp (string place)const override{return place;}
    string translate_cond(string l1, string l2)const override{return l1;}
};

class DeclAST : public BaseAST {
    public:
    bool is_global;
    std::string var_type;
    std::unique_ptr<BaseAST> vardeflist;

    void Print() const override {
    std::cout << "DeclAST :";
    std::cout << var_type << endl;
    vardeflist->Print();
    std::cout << ";" << endl;
    }
    void Dump() const override {
        if(is_global){
            vardeflist->translate_exp("GLOBAL");
        }
        else vardeflist->Dump();
    }
    string translate_exp (string place)const override{return place;}
    string translate_cond(string l1, string l2)const override{return l1;}
};

class VarDefListAST : public BaseAST {
    public:
    bool is_single;
    std::unique_ptr<BaseAST> vardef;
    std::unique_ptr<BaseAST> vardeflist;

    void Print() const override {
        //std::cout << "VarDefListAST :";
        if(is_single){
            vardef->Print();
        }
        else{
            vardeflist->Print();
            vardef->Print();
        }
    }
    void Dump() const override {
        if(is_single){
            vardef->Dump();
        }
        else{
            vardeflist->Dump();
            vardef->Dump();
        }
    }
    string translate_exp (string place)const override{
        if(is_single){
            vardef->translate_exp("GLOBAL");
        }
        else{
            vardeflist->translate_exp("GLOBAL");
            vardef->translate_exp("GLOBAL");
        }
        return place;
    }
    string translate_cond(string l1, string l2)const override{return l1;}
};

// class InitValAST : public BaseAST {
//     public:
//     bool is_exp;
//     std::string ident;
//     int value;
//     std::unique_ptr<BaseAST> exp;
//     std::unique_ptr<BaseAST> initvallist;

//     void Print() const override {
//     std::cout << "InitValAST :";
//     if(is_exp){
//         exp->Print();
//     }
//     else{
//         std::cout << "{";
//         initvallist->Print();
//         std::cout << "}";
//     }
//     std::cout << endl;
//     }
//     void Dump() const override {
        
//     }
//     string translate_exp (string place)const override{
//         string code;
//         if(is_exp){
//             code = exp->translate_exp(place);
//         }
//         return code;
//     }
//     string translate_cond(string l1, string l2)const override{return l1;}
// };

class InitValListAST : public BaseAST {
    public:
    InitValListAST* next;  // 指向下一个节点，值或子列表
    bool isValue;            // 是否为值，0代表子列表，1代表值
    union {          // 值节点的值
            int value;
            InitValListAST* child; // 子列表节点
    } data;
    BaseAST * expr;
    std::unique_ptr<BaseAST> exp;

    void Print() const override {
        std::cout << "{";
        const InitValListAST* current = this;
        while (current != nullptr) {
            if (current->isValue) {
                cout << current->data.value;
                //cout << current->data.expr->translate_exp("EXP is ->");
            } else {
                if(current->data.child == NULL) return;
                else current->data.child->Print();  // 递归打印子列表
            }

            if (current->next != nullptr) {
                std::cout << ", ";
            }

            current = current->next;
        }
        std::cout << "}";
    }

    ~InitValListAST() {
        if (!isValue && data.child != nullptr) {
            delete data.child;
        }
        if (next != nullptr) {
            delete next;
        }
    }
    
    void Dump() const override {
        
    }
    string translate_exp (string place)const override{return place;}
        
    string translate_cond(string l1, string l2)const override{return l1;}
};

class ExpAST : public BaseAST {
    public:
    bool is_lval;
    std::string ident;
    vector<int> bracketlist;
    int value;
    std::unique_ptr<BaseAST> addexp;

    void Print() const override {
        if(addexp != NULL){
            std::cout << "ExpAST : value = " << value << " exp :";
            addexp->Print();
            std::cout << endl;
        }
    }
    void Dump() const override {
        // if(addexp != NULL){
        //     //fprintf(yyout, "%d\n", value);
        //     addexp->Dump();
        // }
    }
    string translate_exp(string place) const override {
        return addexp->translate_exp(place);
    }
    string translate_cond(string l1, string l2)const override{return l1;}
};

class VarDefAST : public BaseAST {
    public:
    bool is_init;
    std::string ident;
    std::unique_ptr<BaseAST> bracketlist;
    std::unique_ptr<BaseAST> initval;


    string zero_fill(InitValListAST* root, int size, int &index, int * val, vector<int> array_size, int length) const{
      InitValListAST* head = root;
      InitValListAST* current = root->data.child;
      string code;
      while(current){
            if(current->isValue){
                ExpAST * t = dynamic_cast<ExpAST *>(current->expr);
                if(t->is_lval && t->ident == "c"){
                    val[index++] = 6;
                }
                  else val[index++] = current->data.value;
            }
            else 
            {
                  int t, mod;
                  for(t = length - 1, mod = 1; t >=  0; t--){
                        mod *= array_size[t];
                        
                        if(index % mod != 0) break;
                        else if(t == 0) break;
                        //else if(index % mod != 0 || t == 0) t = -1;
                  }
                  //if(t == -1) return -1;
                  //cout<< "index : "<<index << " mod : " << mod << endl;
                  int new_index = index + mod / array_size[t];
                  zero_fill(current, new_index, index, val, array_size, t+1);
                  index = new_index;
                   
            }
            current = current->next;
      }
      printf("index : %d, size : %d\n" ,index, size);
      return code;
}

    void Print() const override {
    std::cout << "VarDefAST :";
    if(!is_init){
        std::cout << ident;
        bracketlist->Print();
    }
    else{
        std::cout << ident;
        bracketlist->Print();
        std::cout << " = ";
        InitValListAST * pt = dynamic_cast<InitValListAST *>(initval.get());
        initval->Print();
    }
    std::cout << endl;
    }
    void Dump() const override{
        string t1, t2, t3, t4, t6, code;
        BRACKETListAST * pt = dynamic_cast<BRACKETListAST *>(bracketlist.get());
        InitValListAST * pt3 = dynamic_cast<InitValListAST *>(initval.get());
        vector<int> var = pt->bracketlist;
        meta n = init_meta(VARIABLE, var, {}, 0);
        n->global = 0;
        if(check_redef(ident, table_dump) == false){
                        error_flag = 1;
        }
        else insert(ident, n, table_dump);

        int d = 4;
        meta m = lookup(ident, table_dump);
        if(m == NULL){
            cout << "VarDef Dump fail" << endl;
            exit(0);
        }
        int size = 1;
        if(!is_init){
            if(m->var.size() == 0){// not Array
                m->id = temp_count;
                t1 = generateTemp();
                fprintf(yyout, "%s = #%d\n", t1.c_str(), m->value);
            }
            else{
                for(int i = 0; i < m->var.size(); i++){
                    d *= m->var[i];
                }
                m->id = temp_count;
                t1 = generateTemp();
                fprintf(yyout, "DEC %s #%d\n", t1.c_str(), d);
            }
        }
        else{
            if(m->var.size() == 0){// not Array
                m->id = temp_count;
                t1 = generateTemp();
                code += pt3->exp->translate_exp(t1);
                //cout << "ID is " << ident <<endl;
                fprintf(yyout, "%s", code.c_str());
            }
            else{
                for(int i = 0; i < m->var.size(); i++){
                    size *= m->var[i];
                }
                int* val = new int[2 * size];
                int index = 0;
                code += zero_fill(pt3, size, index, val, m->var, m->var.size());
                //if(zero_fill(pt3, size, index, val, m->var, m->var.size())) 
                //              {
                //              for(int i = 0; i < size; i++)
                //                    printf("%d, ", val[i]);
                //              printf("\n");
                //              error_flag = 1;
                //              cout << "ERROR : init_list not match" <<endl;
                //              exit(0);
                //        }
                
                for(int i = 0; i < m->var.size(); i++){
                    d *= m->var[i];
                }
                m->id = temp_count;
                t1 = generateTemp();
                fprintf(yyout, "DEC %s #%d\n", t1.c_str(), d);
                t2 = generateTemp();
                t3 = generateTemp();
                t4 = generateTemp();
                t6 = generateTemp();
                fprintf(yyout, "%s = #4\n", t2.c_str());
                fprintf(yyout, "%s = %s\n", t4.c_str(), t1.c_str());
                for(int i = 0; i < size; i++){
                    fprintf(yyout, "%s = #%d\n", t6.c_str() ,val[i]);                       //t6 = val[i]
                    fprintf(yyout, "%s = %s + %s\n", t3.c_str(), t4.c_str(), t2.c_str());   //t3 = t4 + #4
                    fprintf(yyout, "*%s = %s\n", t4.c_str(), t6.c_str());                   //*t3 = t6    
                    fprintf(yyout, "%s = %s\n", t4.c_str(), t3.c_str());                   //t4 = t3
                }
                delete[] val;
            }
        }
    }
    string translate_exp (string place)const override{
        string t1, t2, code;
        BRACKETListAST * pt = dynamic_cast<BRACKETListAST *>(bracketlist.get());
        InitValListAST * pt3 = dynamic_cast<InitValListAST *>(initval.get());
        vector<int> var = pt->bracketlist;
        meta n = init_meta(VARIABLE, var, {}, 0);
        n->global = 1;
        if(check_redef(ident, table_dump) == false){
                        error_flag = 1;
        }
        else insert(ident, n, table_dump);

        int d = 1;
        meta m = lookup(ident, table_dump);
        if(m == NULL){
            cout << "VarDef Dump fail" << endl;
            exit(0);
        }
        int size = 1;
        if(!is_init){
            if(m->var.size() == 0){// not Array
                fprintf(yyout, "GLOBAL %s:\n", ident.c_str());
                fprintf(yyout, "    .WORD #%d\n", 0);
            }
            else{
                fprintf(yyout, "GLOBAL %s:\n", ident.c_str());
                for(int i = 0; i < m->var.size(); i++){
                    d *= m->var[i];
                }
                for(int i = 0; i < d; i++){
                    fprintf(yyout, "    .WORD #%d\n", 0);
                }
            }
        }
        else{
            if(m->var.size() == 0){// not Array
                int v = pt3->data.value; 
                fprintf(yyout, "GLOBAL %s:\n", ident.c_str());
                fprintf(yyout, "    .WORD #%d\n", v);
            }
            else{
                for(int i = 0; i < m->var.size(); i++){
                    size *= m->var[i];
                }
                int* val = new int[2 * size];
                int index = 0;
                code += zero_fill(pt3, size, index, val, m->var, m->var.size());
                //if(zero_fill(pt3, size, index, val, m->var, m->var.size())) 
                //              {
                //              for(int i = 0; i < size; i++)
                //                    printf("%d, ", val[i]);
                //              printf("\n");
                //              error_flag = 1;
                //              cout << "ERROR : init_list not match" <<endl;
                //              exit(0);
                //        }
                fprintf(yyout, "GLOBAL %s:\n", ident.c_str());
                for(int i = 0; i < size; i++){
                    fprintf(yyout, "    .WORD #%d\n", val[i]);
                }
                delete[] val;
            }
        
        }
        return place;
    }
    string translate_cond(string l1, string l2)const override{return l1;}
    
};


enum Op{
    NONE,
    Add,
    Mul,
    Sub, 
    Div, 
    Mod,
    Not, 
    Le, 
    Ge, 
    Lt, 
    Gt, 
    Eq, 
    Ne, 
    And, 
    Or,
    FUNCALL
};

class AddExpAST : public BaseAST {
    public:
    bool is_lval;
    std::string ident;
    vector<int> bracketlist;
    int value;
    enum Op op;
    std::unique_ptr<BaseAST> addexp;
    std::unique_ptr<BaseAST> mulexp;

    void Print() const override {
        switch (this->op) {
            case NONE:
                this->mulexp->Print();
                break;
            case Add:
                this->addexp->Print();
                std::cout<<" + ";
                this->mulexp->Print();
                break;
            case Sub:
                this->addexp->Print();
                std::cout<<" - ";
                this->mulexp->Print();
                break;
            default : break;
        }
    }
    void Dump() const override {
        
    }
    string translate_exp(string place) const override {
        std::string code;
        std::string t1, t2, code1, code2;
        switch (this->op) {
            case NONE:
                code = mulexp->translate_exp(place);
                break;
            case Add:
                t1 = generateTemp();
                t2 = generateTemp();
                code1 = this->addexp->translate_exp(t1);
                code2 = this->mulexp->translate_exp(t2);
                code += code1 + code2;
                code += place + " = " + t1 + " + " + t2 + "\n";
                break;
            case Sub:
                t1 = generateTemp();
                t2 = generateTemp();
                code1 = this->addexp->translate_exp(t1);
                code2 = this->mulexp->translate_exp(t2);
                code += code1 + code2;
                code += place + " = " + t1 + " - " + t2 + "\n";
                break;
            default : break;
        }
        return code;
    }
    string translate_cond(string l1, string l2)const override{return l1;}
};

class MulExpAST : public BaseAST {
    public:
    bool is_lval;
    std::string ident;
    vector<int> bracketlist;
    int value;
    enum Op op;
    std::unique_ptr<BaseAST> unaryexp;
    std::unique_ptr<BaseAST> mulexp;

    void Print() const override {
    switch (this->op) {
        case NONE:
            this->unaryexp->Print();
            break;
        case Mul:
            this->mulexp->Print();
            std::cout<<" * ";
            this->unaryexp->Print();
            break;
        case Div:
            this->mulexp->Print();
            std::cout<<" / ";
            this->unaryexp->Print();
            break;
        case Mod:
            this->mulexp->Print();
            std::cout<<" % ";
            this->unaryexp->Print();
            break;
        default : break;
    }
    }
    void Dump() const override {
        
    }
    string translate_exp(string place) const override {
        std::string code;
        std::string t1, t2, code1, code2;
        switch (this->op) {
            case NONE:
                code = unaryexp->translate_exp(place);
                break;
            case Mul:
                t1 = generateTemp();
                t2 = generateTemp();
                code1 = this->mulexp->translate_exp(t1);
                code2 = this->unaryexp->translate_exp(t2);
                code += code1 + code2;
                code += place + " = " + t1 + " * " + t2 + "\n";
                break;
            case Div:
                t1 = generateTemp();
                t2 = generateTemp();
                code1 = this->mulexp->translate_exp(t1);
                code2 = this->unaryexp->translate_exp(t2);
                code += code1 + code2;
                code += place + " = " + t1 + " / " + t2 + "\n";
                break;
            case Mod:
                t1 = generateTemp();
                t2 = generateTemp();
                code1 = this->mulexp->translate_exp(t1);
                code2 = this->unaryexp->translate_exp(t2);
                code += code1 + code2;
                code += place + " = " + t1 + " % " + t2 + "\n";
                break;
            default : break;
        }
        return code;
    }
    string translate_cond(string l1, string l2)const override{return l1;}
};

class UnaryExpAST : public BaseAST {
    public:
    bool is_lval;
    std::string ident;
    vector<int> bracketlist;
    int value;
    enum Op op;
    std::unique_ptr<BaseAST> primaryexp;
    std::unique_ptr<BaseAST> unaryexp;
    std::unique_ptr<BaseAST> funcall;

    void Print() const override {
    switch (this->op) {
        case NONE:
            this->primaryexp->Print();
            break;
        case Add:
            std::cout<<" +";
            this->unaryexp->Print();
            break;
        case Sub:
            std::cout<<" -";
            this->unaryexp->Print();
            break;
        case Not:
            std::cout<<" !";
            this->unaryexp->Print();
            break;
        case FUNCALL:
            this->funcall->Print();
            break;
        default : break;
    }
    }
    void Dump() const override {
        
    }
    string translate_exp(string place) const override {
        std::string code;
        std::string t1, t2, code1, code2;
        std::string l1, l2, l3;
        switch (this->op) {
            case NONE:
                code = primaryexp->translate_exp(place);
                break;
            case Add:
                code = unaryexp->translate_exp(place);
                break;
            case Sub:
                t1 = generateTemp();
                code1 = this->unaryexp->translate_exp(t1);
                code += code1;
                code += place + " = - " + t1 + "\n";
                break;
            case Not:
                t1 = generateTemp();
                t2 = generateTemp();
                l1 = new_label();
                l2 = new_label();
                l3 = new_label();
                code1 = this->unaryexp->translate_exp(t1);
                code += code1;
                code += t2 + " = #0\n";
                code += "IF " + t1 + " == " + t2 + " GOTO " + l2 + "\n";
                code += "LABEL " + l1 + ":\n";
                code += place + " = #0\n";
                code += "GOTO " + l3 + "\n";
                code += "LABEL " + l2 + ":\n";
                code += place + " = #1\n";
                code += "LABEL " + l3 + ":\n";
                break;
            case FUNCALL:
                code = funcall->translate_exp(place);
            break;
            default : break;
        }
        return code;
    }
    string translate_cond(string l1, string l2)const override{return l1;}
};

enum type{
        EXP,
        Lval,
        Num
    };

class PrimaryExpAST : public BaseAST{
    public:
    enum type type;
    std::string ident;
    vector<int> bracketlist;
    vector<BaseAST *> explist;
    int value;
    std::unique_ptr<BaseAST> exp;
    void Print() const override {
        switch (this->type) {
        case EXP:
            std::cout << " (";
            this->exp->Print();
            std::cout << ") ";
            break;
        case Lval:
            std::cout << ident;
            for (int i = 0; i < this->bracketlist.size(); i++) {
                cout << "["<< bracketlist[i] << "]";
            }
            break;
        case Num:
            std::cout << this->value;
            break;
        default : break;
    }
    }
    void Dump() const override {
        
    }
    string translate_exp(string place) const override{
        std::string code;
        std::string t1, t2, code1, code2;
        std::string p1, p2, p3, p4, p5, p6, p7;
        int v;
        meta m;
        int d = 4, offset = 0;
        switch (this->type) {
            case EXP:
                t1 = generateTemp();
                code1 = this->exp->translate_exp(t1);
                code += code1;
                code += place + " = " + t1 + "\n";
                break;
            case Lval:
                m = lookup(ident, table_dump);
                //cout << "ID is : "<<ident << " is global :" <<m->global <<endl;
                if(m == NULL){
                    cout << "PrimalExp not find variable :" << ident << endl;
                    exit(0);
                }
                if(m->global){
                    if(m->var.size() == 0){
                        t1 = generateTemp();
                        code += t1 + " = &" + ident + "\n";
                        code += place + " = *" + t1 + "\n";
                    }
                    else{
                        if(explist.size() < m->var.size()){
                            for(int i = explist.size(); i < m->var.size(); i++){
                                d *= m->var[i];
                            }
                            p1 = generateTemp();
                            p2 = generateTemp();
                            p3 = generateTemp();
                            p4 = generateTemp();
                            p5 = generateTemp();
                            p6 = generateTemp();
                            p7 = generateTemp();
                            code += p3 + " = #" + to_string(d) +"\n";   //p3 = d
                            code += p2 + " = #0\n"; //p2 = offset
                            for(int i = explist.size() - 1; i >= 0; i--){
                                ExpAST * pt = dynamic_cast<ExpAST *>(explist[i]);
                                
                                code += pt->translate_exp(p1);
                                code += p6 + " = " + p2 + "\n";
                                code += p7 + " = " + p3 + " * " + p1 + "\n";   //
                                code += p2 + " = " + p6 + " + " + p7 + "\n";
                                // offset += d * bracketlist[i];
                                code += p4 + " = #" + to_string(m->var[i]) + "\n";
                                code += p5 + " = " + p4 + " * " + p3 + "\n";
                                code += p3 + " = " + p5 + "\n";
                                // d *= m->var[i];
                            }
                            //t1 = generateTemp();
                            t2 = generateTemp();
                            //code += t1 + " = #" + to_string(offset) + "\n";
                            code += t2 + " = &" + ident + "\n";
                            code += place + " = " + t2 + " + " + p2 + "\n";
                        }
                        else{
                            for(int i = explist.size(); i < m->var.size(); i++){
                                d *= m->var[i];
                            }
                            p1 = generateTemp();
                            p2 = generateTemp();
                            p3 = generateTemp();
                            p4 = generateTemp();
                            p5 = generateTemp();
                            p6 = generateTemp();
                            p7 = generateTemp();
                            code += p3 + " = #" + to_string(d) +"\n";   //p3 = d
                            code += p2 + " = #0\n"; //p2 = offset
                            for(int i = explist.size() - 1; i >= 0; i--){
                                ExpAST * pt = dynamic_cast<ExpAST *>(explist[i]);
                                
                                code += pt->translate_exp(p1);
                                code += p6 + " = " + p2 + "\n";
                                code += p7 + " = " + p3 + " * " + p1 + "\n";   //
                                code += p2 + " = " + p6 + " + " + p7 + "\n";
                                // offset += d * bracketlist[i];
                                code += p4 + " = #" + to_string(m->var[i]) + "\n";
                                code += p5 + " = " + p4 + " * " + p3 + "\n";
                                code += p3 + " = " + p5 + "\n";
                                // d *= m->var[i];
                            }
                            t1 = generateTemp();
                            t2 = generateTemp();
                            //code += t1 + " = #" + to_string(offset) + "\n";
                            code += t2 + " = &" + ident + "\n";
                            code += t1 + " = " + p2 + " + " + t2 + "\n";
                            code += place + " = *" + t1 + "\n"; 
                        }
                    }
                }
                else{
                    if(m->var.size() == 0){
                        code += place + " = t" + to_string(m->id) + "\n";
                    }
                    else{
                        if(explist.size() < m->var.size()){
                            for(int i = explist.size(); i < m->var.size(); i++){
                                d *= m->var[i];
                            }
                            p1 = generateTemp();
                            p2 = generateTemp();
                            p3 = generateTemp();
                            p4 = generateTemp();
                            p5 = generateTemp();
                            p6 = generateTemp();
                            p7 = generateTemp();
                            code += p3 + " = #" + to_string(d) +"\n";   //p3 = d
                            code += p2 + " = #0\n"; //p2 = offset
                            for(int i = explist.size() - 1; i >= 0; i--){
                                ExpAST * pt = dynamic_cast<ExpAST *>(explist[i]);
                                
                                code += pt->translate_exp(p1);
                                code += p6 + " = " + p2 + "\n";
                                code += p7 + " = " + p3 + " * " + p1 + "\n";   //
                                code += p2 + " = " + p6 + " + " + p7 + "\n";
                                // offset += d * bracketlist[i];
                                code += p4 + " = #" + to_string(m->var[i]) + "\n";
                                code += p5 + " = " + p4 + " * " + p3 + "\n";
                                code += p3 + " = " + p5 + "\n";
                                // d *= m->var[i];
                            }
                            //t1 = generateTemp();
                            //code += t1 + " = #" + to_string(offset) + "\n";
                            code += place + " = t" + to_string(m->id) + " + " + p2 + "\n";
                        }
                        else{
                            for(int i = explist.size(); i < m->var.size(); i++){
                                d *= m->var[i];
                            }
                            p1 = generateTemp();
                            p2 = generateTemp();
                            p3 = generateTemp();
                            p4 = generateTemp();
                            p5 = generateTemp();
                            p6 = generateTemp();
                            p7 = generateTemp();
                            code += p3 + " = #" + to_string(d) +"\n";   //p3 = d
                            code += p2 + " = #0\n"; //p2 = offset
                            for(int i = explist.size() - 1; i >= 0; i--){
                                ExpAST * pt = dynamic_cast<ExpAST *>(explist[i]);
                                
                                code += pt->translate_exp(p1);
                                code += p6 + " = " + p2 + "\n";
                                code += p7 + " = " + p3 + " * " + p1 + "\n";   //
                                code += p2 + " = " + p6 + " + " + p7 + "\n";
                                // offset += d * bracketlist[i];
                                code += p4 + " = #" + to_string(m->var[i]) + "\n";
                                code += p5 + " = " + p4 + " * " + p3 + "\n";
                                code += p3 + " = " + p5 + "\n";
                                // d *= m->var[i];
                            }
                            t1 = generateTemp();
                            //code += t1 + " = #" + to_string(offset) + "\n";
                            code += t1 + " = " + p2 + " + t" + to_string(m->id) + "\n";
                            code += place + " = *" + t1 + "\n"; 
                        }
                    }
                }
                break;
            case Num:
                code += place + " = #" + to_string(value) + "\n";
                break;
            default : break;
        }
        return code;
    }
    string translate_cond(string l1, string l2)const override{return l1;}
};

class CondAST : public BaseAST{
    public:
    std::unique_ptr<BaseAST> lorexp;
    void Print() const override {
        std::cout << "CondAST : ";
        lorexp->Print();
    }
    void Dump() const override {
        
    }
    string translate_exp (string place)const override{
        std::string code;
        std::string t1, t2, code1, code2;
        string l1, l2;

        // size_t pos = place.find(' ');
        // // 如果找到空格
        // if (pos != string::npos) {
        //     // 分割字符串
        //     l1 = place.substr(0, pos);
        //     l2 = place.substr(pos + 1);
        // }

        code = lorexp->translate_exp(place);
        // t1 = generateTemp();
        // code += lorexp->translate_exp(t1);
        // code += "IF " + t1 + " != #0"+ " GOTO " + l1 + "\n";
        // code += "GOTO " + l2 + "\n";
        return code;
    }
    string translate_cond(string l1, string l2)const override{
        std::string code;
        // std::string t1, t2, code1, code2;
        // string l1, l2;

        // size_t pos = place.find(' ');
        // // 如果找到空格
        // if (pos != string::npos) {
        //     // 分割字符串
        //     l1 = place.substr(0, pos);
        //     l2 = place.substr(pos + 1);
        // }

        code = lorexp->translate_cond(l1, l2);
        return code;
    }
};

class LOrExpAST : public BaseAST {
    public:
    bool is_lval;
    std::string ident;
    vector<int> bracketlist;
    int value;
    enum Op op;
    std::unique_ptr<BaseAST> landexp;
    std::unique_ptr<BaseAST> lorexp;

    void Print() const override {
    switch (this->op) {
        case NONE:
            this->landexp->Print();
            break;
        case Or:
            this->lorexp->Print();
            std::cout << " || ";
            this->landexp->Print();
            break;
        default : break;
    }
    }
    void Dump() const override {
        
    }
    string translate_exp (string place)const override{
        // std::string code;
        // std::string t1, t2, code1, code2;
        // std::string l1, l2, lbl;
        // size_t pos = place.find(' ');
        // // 如果找到空格
        // if (pos != string::npos) {
        //     // 分割字符串
        //     l1 = place.substr(0, pos);
        //     l2 = place.substr(pos + 1);
        // }

        // switch (this->op) {
        //     case NONE:
        //         code = landexp->translate_exp(place);
        //         break;
        //     case Or:
        //         lbl = new_label();
        //         code1 = lorexp->translate_exp(l1+" "+lbl);
        //         code2 = landexp->translate_exp(l1+" "+l2);
        //         code += code1 + "LABEL " + lbl + ":\n" + code2;
        //         break;
        //     default : break;
        // }
        // return code;
    }
    string translate_cond(string l1, string l2)const override{
        std::string code;
        std::string t1, t2, code1, code2;
        std::string lbl;
        switch (this->op) {
            case NONE:
                code = landexp->translate_cond(l1, l2);
                break;
            case Or:
                lbl = new_label();
                code1 = lorexp->translate_cond(l1, lbl);
                code2 = landexp->translate_cond(l1, l2);
                code += code1 + "LABEL " + lbl + ":\n" + code2;
                break;
            default : break;
        }
        return code;
    }
};

class LAndExpAST : public BaseAST {
    public:
    bool is_lval;
    std::string ident;
    vector<int> bracketlist;
    int value;
    enum Op op;
    std::unique_ptr<BaseAST> eqexp;
    std::unique_ptr<BaseAST> landexp;

    void Print() const override {
    switch (this->op) {
        case NONE:
            this->eqexp->Print();
            break;
        case And:
            this->landexp->Print();
            std::cout << " && ";
            this->eqexp->Print();
            break;
        default : break;
    }
    }
    void Dump() const override {
        
    }
    string translate_exp (string place)const override{
    //     std::string code;
    //     std::string t1, t2, code1, code2;
    //     std::string l1, l2, lbl;
    //     size_t pos = place.find(' ');
    //     // 如果找到空格
    //     if (pos != string::npos) {
    //         // 分割字符串
    //         l1 = place.substr(0, pos);
    //         l2 = place.substr(pos + 1);
    //     }
    //     switch (this->op) {
    //     case NONE:
    //         code = eqexp->translate_exp(place);
    //         break;
    //     case And:
    //         lbl = new_label();
    //         code1 = landexp->translate_exp(lbl+" "+l2);
    //         code2 = eqexp->translate_exp(l1+" "+l2);
    //         code += code1 + "LABEL " + lbl + ":\n" + code2;
    //         break;
    //     default : break;
    // }
    }
    string translate_cond(string l1, string l2)const override{
        std::string code;
        std::string t1, t2, code1, code2;
        std::string lbl;
        switch (this->op) {
        case NONE:
            code = eqexp->translate_cond(l1, l2);
            break;
        case And:
            lbl = new_label();
            code1 = landexp->translate_cond(lbl, l2);
            code2 = eqexp->translate_cond(l1, l2);
            code += code1 + "LABEL " + lbl + ":\n" + code2;
            break;
        default : break;
    }
    return code;
    }
};

class EqExpAST : public BaseAST {
    public:
    bool is_lval;
    std::string ident;
    vector<int> bracketlist;
    int value;
    enum Op op;
    std::unique_ptr<BaseAST> eqexp;
    std::unique_ptr<BaseAST> relexp;

    void Print() const override {
    switch (this->op) {
        case NONE:
            this->relexp->Print();
            break;
        case Eq:
            this->eqexp->Print();
            std::cout << " == ";
            this->relexp->Print();
            break;
        case Ne:
            this->eqexp->Print();
            std::cout << " != ";
            this->relexp->Print();
            break;
        default : break;
    }
    }
    void Dump() const override {
        
    }
    string translate_exp (string place)const override{
        std::string code;
        std::string t1, t2, code1, code2;       
        switch (this->op) {
        case NONE:
            code = relexp->translate_exp(place);
            break;
        case Eq:
            t1 = generateTemp();
            t2 = generateTemp();
            code1 = eqexp->translate_exp(t1);
            code2 = relexp->translate_exp(t2);
            code += code1 + code2;
            code += place + " = " + t1 + " - " + t2;
            break;
        case Ne:
            t1 = generateTemp();
            t2 = generateTemp();
            code1 = eqexp->translate_exp(t1);
            code2 = relexp->translate_exp(t2);
            code += code1 + code2;
            code += place + " = " + t1 + " - " + t2;
            break;
        default : break;
    }
    return code;
    }
    string translate_cond(string l1, string l2)const override{
        std::string code;
        std::string t1, t2, code1, code2;
        std::string lbl;
        switch (this->op) {
        case NONE:
            code = relexp->translate_cond(l1, l2);
            break;
        case Eq:
            t1 = generateTemp();
            t2 = generateTemp();
            code1 = eqexp->translate_exp(t1);
            code2 = relexp->translate_exp(t2);
            code += code1 + code2;
            code += "IF " + t1 + " == " + t2 + " GOTO " + l1 + "\n";
            code += "GOTO " + l2 + "\n";
            break;
        case Ne:
            t1 = generateTemp();
            t2 = generateTemp();
            code1 = eqexp->translate_exp(t1);
            code2 = relexp->translate_exp(t2);
            code += code1 + code2;
            code += "IF " + t1 + " != " + t2 + " GOTO " + l1 + "\n";
            code += "GOTO " + l2 + "\n";
            break;
        default : break;
    }
    return code;
    }
};

class RelExpAST : public BaseAST {
    public:
    bool is_lval;
    std::string ident;
    vector<int> bracketlist;
    int value;
    enum Op op;
    std::unique_ptr<BaseAST> addexp;
    std::unique_ptr<BaseAST> relexp;

    void Print() const override {
    switch (this->op) {
        case NONE:
            this->addexp->Print();
            break;
        case Lt:
            this->relexp->Print();
            std::cout << " < ";
            this->addexp->Print();
            break;
        case Gt:
            this->relexp->Print();
            std::cout << " > ";
            this->addexp->Print();
            break;
        case Le:
            this->relexp->Print();
            std::cout << " <= ";
            this->addexp->Print();
            break;
        case Ge:
            this->relexp->Print();
            std::cout << " >= ";
            this->addexp->Print();
            break;
        default : break;
    }
    }
    void Dump() const override {
        
    }
    string translate_exp (string place)const override{
        std::string code;
        std::string t1, t2, code1, code2;       
        switch (this->op) {
        case NONE:
            code = addexp->translate_exp(place);
            break;
        case Lt:
            break;
        case Gt:
            break;
        case Le:
            break;
        case Ge:
            break;
        default : break;
    }
    return code;
    }
    string translate_cond(string l1, string l2)const override{
        std::string code;
        std::string t1, t2, code1, code2;
        std::string lbl;
        switch (this->op) {
        case NONE:
            t1 = generateTemp();
            t2 = generateTemp();
            code = addexp->translate_exp(t1);
            code += t2 + " = #0\n" ;
            code += "IF " + t1 + " != "+ t2 + " GOTO " + l1 + "\n";
            code += "GOTO " + l2 + "\n";
            break;
        case Lt:
            t1 = generateTemp();
            t2 = generateTemp();
            code1 = relexp->translate_exp(t1);
            code2 = addexp->translate_exp(t2);
            code += code1 + code2;
            code += "IF " + t1 + " < " + t2 + " GOTO " + l1 + "\n";
            code += "GOTO " + l2 + "\n";
            break;
        case Gt:
            t1 = generateTemp();
            t2 = generateTemp();
            code1 = relexp->translate_exp(t1);
            code2 = addexp->translate_exp(t2);
            code += code1 + code2;
            code += "IF " + t1 + " > " + t2 + " GOTO " + l1 + "\n";
            code += "GOTO " + l2 + "\n";
            break;
        case Le:
            t1 = generateTemp();
            t2 = generateTemp();
            code1 = relexp->translate_exp(t1);
            code2 = addexp->translate_exp(t2);
            code += code1 + code2;
            code += "IF " + t1 + " <= " + t2 + " GOTO " + l1 + "\n";
            code += "GOTO " + l2 + "\n";
            break;
        case Ge:
            t1 = generateTemp();
            t2 = generateTemp();
            code1 = relexp->translate_exp(t1);
            code2 = addexp->translate_exp(t2);
            code += code1 + code2;
            code += "IF " + t1 + " >= " + t2 + " GOTO " + l1 + "\n";
            code += "GOTO " + l2 + "\n";
            break;
        default : break;
    }
    return code;
    }
};

class FunCallAST : public BaseAST{
    public:
    std::string ident;
    std::unique_ptr<BaseAST> funparams;
    void Print() const override {
        std::cout << "FunCallAST : ";
        std::cout << ident ;
        std::cout << ": (";
        funparams->Print();
        std::cout << ")\n";
    }
    void Dump() const override {
        // funparams->Dump();
        // fprintf(yyout, "CALL %s", ident.c_str());
    }
    string translate_exp(string place) const override {
        std::string code;
        std::string t1, t2, code1, code2;
        code += funparams->translate_exp("ARG");
        code += place + " = CALL " + ident + "\n";
        return code;
    }
    string translate_cond(string l1, string l2)const override{return l1;}
};


class BlockItemsAST : public BaseAST{
    public:
    bool empty;
    std::unique_ptr<BaseAST> block;
    std::unique_ptr<BaseAST> blockitems;
    void Print() const override {
        //std::cout << "BlockItemsAST :";
        if(!this->empty){
            blockitems->Print();
            block->Print();
        }
        std::cout << endl;
    }
    void Dump() const override {
        //std::cout << "BlockItemsAST :";
        if(!this->empty){
            blockitems->Dump();
            block->Dump();
        }
    }
    string translate_exp (string place)const override{return place;}
    string translate_cond(string l1, string l2)const override{return l1;}
};

class BlockItemAST : public BaseAST{
    public:
    bool is_decl;
    std::unique_ptr<BaseAST> decl;
    std::unique_ptr<BaseAST> stmt;
    void Print() const override {
        if(this->is_decl){
            decl->Print();
        }
        else stmt->Print();
        std::cout << endl;
    }
    void Dump() const override {
        if(this->is_decl){
            decl->Dump();
        }
        else stmt->Dump();
    }
    string translate_exp (string place)const override{return place;}
    string translate_cond(string l1, string l2)const override{return l1;}
};

class StmtAST : public BaseAST{
    public:
    bool is_open;
    std::unique_ptr<BaseAST> matched;
    std::unique_ptr<BaseAST> open;
    void Print() const override {
        // std::cout << "StmtAST :";
        if(this->is_open){
            open->Print();
        }
        else matched->Print();
        std::cout << endl;
    }
    void Dump() const override {
        if(this->is_open){
            open->Dump();
        }
        else matched->Dump();
    }
    string translate_exp (string place)const override{return place;}
    string translate_cond(string l1, string l2)const override{return l1;}
};

class MatchedStmtAST : public BaseAST{
    public:
    bool is_if;
    std::unique_ptr<BaseAST> cond;
    std::unique_ptr<BaseAST> matched1;
    std::unique_ptr<BaseAST> matched2;
    std::unique_ptr<BaseAST> noif;
    void Print() const override {
        //std::cout << "StmtAST :";
        if(this->is_if){
            std::cout << "IF (";
            cond->Print();
            std::cout << ")" << endl;
            matched1->Print();
            std::cout << "\nELSE";
            matched2->Print();
        }
        else noif->Print();
        std::cout << endl;
    }
    void Dump() const override {
        string code1, l1, l2, l3;
        if(this->is_if){
            l1 = new_label();
            l2 = new_label();
            l3 = new_label();
            code1 += cond->translate_cond(l1, l2);
            code1 += "LABEL " + l1 + ":\n";
            fprintf(yyout, "%s", code1.c_str());
            matched1->Dump();
            fprintf(yyout, "GOTO %s\n", l3.c_str());
            fprintf(yyout, "LABEL %s:\n", l2.c_str());
            matched2->Dump();
            fprintf(yyout, "LABEL %s:\n", l3.c_str());
        }
        else noif->Dump();
    }
    string translate_exp (string place)const override{return place;}
    string translate_cond(string l1, string l2)const override{return l1;}
};

class OpenStmtAST : public BaseAST{
    public:
    bool is_match;
    std::unique_ptr<BaseAST> cond;
    std::unique_ptr<BaseAST> stmt;
    std::unique_ptr<BaseAST> matched;
    std::unique_ptr<BaseAST> open;
    void Print() const override {
        //std::cout << "StmtAST :";
        if(!(this->is_match)){
            std::cout << "IF (";
            cond->Print();
            std::cout << ")" << endl;
            stmt->Print();
        }
        else{
            std::cout << "IF (";
            cond->Print();
            std::cout << ")" << endl;
            matched->Print();
            std::cout << "\nELSE";
            open->Print();
        }
        std::cout << endl;
    }
    void Dump() const override {
        string code1, l1, l2, l3;
        if(!(this->is_match)){
            l1 = new_label();
            l2 = new_label();
            code1 += cond->translate_cond(l1, l2);
            code1 += "LABEL " + l1 + ":\n";
            fprintf(yyout, "%s", code1.c_str());
            stmt->Dump();
            fprintf(yyout, "LABEL %s:\n", l2.c_str());
        }
        else{
            l1 = new_label();
            l2 = new_label();
            l3 = new_label();
            code1 += cond->translate_cond(l1, l2);
            code1 += "LABEL " + l1 + ":\n";
            fprintf(yyout, "%s", code1.c_str());
            matched->Dump();
            fprintf(yyout, "GOTO %s\n", l3.c_str());
            fprintf(yyout, "LABEL %s:\n", l2.c_str());
            open->Dump();
            fprintf(yyout, "LABEL %s:\n", l3.c_str());
        }
    }
    string translate_exp (string place)const override{return place;}
    string translate_cond(string l1, string l2)const override{return l1;}
};
enum nonifstmt{
    lv,
    opt,
    lbr,
    whi,
    ret
};
class NonIfStmtAST : public BaseAST{
    public:
    enum nonifstmt type;
    std::unique_ptr<BaseAST> lval;
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> blockitems;
    std::unique_ptr<BaseAST> cond;
    std::unique_ptr<BaseAST> stmt;
    void Print() const override {
        switch (this->type) {
        case lv:
            lval->Print();
            std::cout<<" = ";
            exp->Print();
            std::cout<<";";
            break;
        case opt:
            if(exp!=NULL)
                exp->Print();
            std::cout<<";";
            break;
        case lbr:
            std::cout<<"{ ";
            blockitems->Print();
            std::cout<<" }";
            break;
        case whi:
            std::cout<<"WHILE (";
            cond->Print();
            std::cout<<")\n";
            stmt->Print();
            break;
        case ret:
            std::cout<<"RETURN ";
            if(exp!=NULL)
                exp->Print();
            std::cout<<";";
            break;
        default : break;
    }
    std::cout << endl;
    }
    void Dump() const override {
        string code, code1, t1, t2, t3, t4, id;
        string l1, l2, l3;
        string p1, p2, p3, p4, p5, p6, p7;
        meta m;
        FuncParamAST * p;
        int d = 4, offset = 0;
        switch (this->type) {
        case lv:
            t1 = generateTemp();
            code1 = exp->translate_exp(t1);
            fprintf(yyout, "%s", code1.c_str());
            p = dynamic_cast<FuncParamAST *>(lval.get());
            id = p->ident;
            m = lookup(id, table_dump);
            if(m == NULL){
                    cout << "NoifStmt not find variable :" << id << endl;
                    exit(0);
                }
            if(m->global){
                if(p->explist.size() == 0){
                    t2 = generateTemp();
                    fprintf(yyout, "%s = &%s\n", t2.c_str(), id.c_str());
                    fprintf(yyout, "*%s = %s\n", t2.c_str(), t1.c_str());
                }
                else{
                    for(int i = p->explist.size(); i < m->var.size(); i++){
                        d *= m->var[i];
                    }
                    p1 = generateTemp();
                    p2 = generateTemp();
                    p3 = generateTemp();
                    p4 = generateTemp();
                    p5 = generateTemp();
                    p6 = generateTemp();
                    p7 = generateTemp();
                    code += p3 + " = #" + to_string(d) +"\n";   //p3 = d
                    code += p2 + " = #0\n"; //p2 = offset
                    for(int i = p->explist.size() - 1; i >= 0; i--){
                        ExpAST * pt = dynamic_cast<ExpAST *>(p->explist[i]);
                        
                        code += pt->translate_exp(p1);
                        code += p6 + " = " + p2 + "\n";
                        code += p7 + " = " + p3 + " * " + p1 + "\n";   //
                        code += p2 + " = " + p6 + " + " + p7 + "\n";
                        // offset += d * bracketlist[i];
                        code += p4 + " = #" + to_string(m->var[i]) + "\n";
                        code += p5 + " = " + p4 + " * " + p3 + "\n";
                        code += p3 + " = " + p5 + "\n";
                        // d *= m->var[i];
                    }
                    //t2 = generateTemp();
                    t3 = generateTemp();
                    t4 = generateTemp();
                    //code += t2 + " = #" + to_string(offset) + "\n";
                    code += t3 + " = &" + id + "\n";
                    code += t4 + " = " + p2 + " + " + t3 + "\n";
                    fprintf(yyout, "%s", code.c_str());
                    fprintf(yyout, "*%s = %s\n", t4.c_str(), t1.c_str());
                }
            }
            else
            {
                if(p->explist.size() == 0){ 
                    fprintf(yyout, "t%d = %s\n", m->id, t1.c_str());
                }
                else{
                    for(int i = p->explist.size(); i < m->var.size(); i++){
                        d *= m->var[i];
                    }
                    p1 = generateTemp();
                    p2 = generateTemp();
                    p3 = generateTemp();
                    p4 = generateTemp();
                    p5 = generateTemp();
                    p6 = generateTemp();
                    p7 = generateTemp();
                    code += p3 + " = #" + to_string(d) +"\n";   //p3 = d
                    code += p2 + " = #0\n"; //p2 = offset
                    for(int i = p->explist.size() - 1; i >= 0; i--){
                        ExpAST * pt = dynamic_cast<ExpAST *>(p->explist[i]);
                        
                        code += pt->translate_exp(p1);
                        code += p6 + " = " + p2 + "\n";
                        code += p7 + " = " + p3 + " * " + p1 + "\n";   //
                        code += p2 + " = " + p6 + " + " + p7 + "\n";
                        // offset += d * bracketlist[i];
                        code += p4 + " = #" + to_string(m->var[i]) + "\n";
                        code += p5 + " = " + p4 + " * " + p3 + "\n";
                        code += p3 + " = " + p5 + "\n";
                        // d *= m->var[i];
                    }
                    //t2 = generateTemp();
                    t3 = generateTemp();
                    //code += t2 + " = #" + to_string(offset) + "\n";
                    code += t3 + " = " + p2 + " + t" + to_string(m->id) + "\n";
                    fprintf(yyout, "%s", code.c_str());
                    fprintf(yyout, "*%s = %s\n", t3.c_str(), t1.c_str());
                }
            }
            break;
        case opt:
            if(exp!=NULL)
            {
            t1 = generateTemp();
            code = exp->translate_exp(t1);
            fprintf(yyout, "%s", code.c_str());
            }
            break;
        case lbr:
            beginscope(table_dump);
            blockitems->Dump();
            endscope(table_dump);
            break;
        case whi:
            l1 = new_label();
            l2 = new_label();
            l3 = new_label();
            code += "LABEL " + l1 +":\n";
            code += cond->translate_cond(l2, l3);
            code += "LABEL " + l2 +":\n";
            fprintf(yyout, "%s", code.c_str());
            stmt->Dump();
            fprintf(yyout, "GOTO %s\n", l1.c_str());
            fprintf(yyout, "LABEL %s:\n", l3.c_str());
            break;
        case ret:
            //fprintf(yyout, "RETURN ");
            if(exp!=NULL)
            {
                t1 = generateTemp();
                code = exp->translate_exp(t1);
                code += "RETURN " + t1 + "\n";
                fprintf(yyout, "%s", code.c_str());
            }
            else fprintf(yyout, "RETURN\n");
            break;
        default : break;
    }
    }
    string translate_exp (string place)const override{return place;}
    string translate_cond(string l1, string l2)const override{return l1;}
};

