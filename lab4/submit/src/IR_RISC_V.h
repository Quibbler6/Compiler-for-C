#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

// Helper function to parse the intermediate code and translate it into RISC-V assembly
std::vector<std::string> translateToRISC(const std::vector<std::string>& intermediateCode) {
    std::vector<std::string> riscvCode;
    std::vector<std::string> dataCode;
    std::unordered_map<std::string, int> stackOffset;
    std::unordered_map<std::string, int> paramsOffset;
    std::vector<int> tempqueue;
    int offsetCounter = 0;
    int paramscounter = 0;
    int argcounter = 0;
    int reg_cn = 0;

    // Add the initial assembly setup
    riscvCode.push_back(".text");
    riscvCode.push_back("_minilib_start:");
    riscvCode.push_back("    la sp, _stack_top");
    riscvCode.push_back("    mv gp, sp");
    riscvCode.push_back("    lui, t3, 0x100");
    riscvCode.push_back("    sub, t3, x0, t3");
    riscvCode.push_back("    add gp, gp, t3");
    riscvCode.push_back("    call main");
    riscvCode.push_back("    mv a1, a0");
    riscvCode.push_back("    li a0, 17");
    riscvCode.push_back("    ecall");
    riscvCode.push_back("read:");
    riscvCode.push_back("    li a0, 6");
    riscvCode.push_back("    ecall");
    riscvCode.push_back("    ret");
    riscvCode.push_back("write:");
    riscvCode.push_back("    mv a1, a0");
    riscvCode.push_back("    li a0, 1");
    riscvCode.push_back("    ecall");
    riscvCode.push_back("    ret");
    // riscvCode.push_back("sbrk:");
    // riscvCode.push_back("    mv a1, a0");
    // riscvCode.push_back("    li a0, 9");
    // riscvCode.push_back("    ecall");
    // riscvCode.push_back("    ret");
    riscvCode.push_back("# Compiled .text continues here");

    for (const auto& line : intermediateCode) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "GLOBAL") {
            std::string var;
            iss >> var;
            dataCode.push_back(".data");
            dataCode.push_back(var);
        } else if (token == ".WORD") {
            std::string value;
            iss >> value;
            dataCode.push_back(".word " + value.substr(1));
        } else if (token == "FUNCTION") {
            std::string func;
            iss >> func;
            riscvCode.push_back(".text");
            riscvCode.push_back(func);
            riscvCode.push_back("li t0, -2048");
            riscvCode.push_back("add sp, sp, t0");
            riscvCode.push_back("sw ra, 0(sp)");
            riscvCode.push_back("sw s2, 4(sp)");    // last frame's stack pointer
            riscvCode.push_back("addi s2, sp, 8");
            paramscounter = 0;
            tempqueue.clear();
        } else if (token == "RETURN") {
            std::string var;
            iss >> var;
            if(var[0] == 't'){
                riscvCode.push_back("lw a0, " + std::to_string(stackOffset[var]) + "(gp)");
            }
            else riscvCode.push_back("li a0, 0");

            riscvCode.push_back("lw ra, 0(sp)");
            riscvCode.push_back("lw s2, 4(sp)");
            riscvCode.push_back("li t0, 2048");
            riscvCode.push_back("add sp, sp, t0");
            riscvCode.push_back("jr ra");
        } else if (token == "CALL") {
            std::string func;
            iss >> func;
            riscvCode.push_back("jal " + func);
            argcounter = 0;
        } else if (token == "ARG") {
            std::string var;
            iss >> var;
            if(argcounter < 8)
                riscvCode.push_back("lw a"+ std::to_string(argcounter++) + ", " + std::to_string(stackOffset[var]) + "(gp)");
            else{
                argcounter-=8;
                riscvCode.push_back("lw s"+ std::to_string(argcounter++) + ", " + std::to_string(stackOffset[var]) + "(gp)");
            }
        } else if (token == "PARAM") {
            std::string var;
            iss >> var;
            paramsOffset[var] = paramscounter++;
            if(paramscounter <= 8)
                riscvCode.push_back("sw a" + std::to_string(paramsOffset[var]) + ", " + std::to_string(4*paramsOffset[var]+8) + "(sp)");
            else{
                paramscounter-=8;
                riscvCode.push_back("sw s"+ std::to_string(paramsOffset[var]-8) + ", " + std::to_string(stackOffset[var]) + "(gp)");
            }
            riscvCode.push_back("addi s2, s2, 4");
        } else if (token == "IF") {
            std::string cond, x, op, y, gotoLabel;
            iss >> x >> op >> y >> gotoLabel >> gotoLabel;
            riscvCode.push_back("lw t3, " + std::to_string(stackOffset[x]) + "(gp)");
            riscvCode.push_back("lw t4, " + std::to_string(stackOffset[y]) + "(gp)");

            if (op == ">") {
                riscvCode.push_back("bgt t3, t4, " + gotoLabel);
            } else if (op == ">=") {
                riscvCode.push_back("bge t3, t4, " + gotoLabel);
            } else if (op == "<") {
                riscvCode.push_back("blt t3, t4, " + gotoLabel);
            } else if (op == "<=") {
                riscvCode.push_back("ble t3, t4, " + gotoLabel);
            } else if (op == "==") {
                riscvCode.push_back("beq t3, t4, " + gotoLabel);
            } else if (op == "!=") {
                riscvCode.push_back("bne t3, t4, " + gotoLabel);
            }
        } else if (token == "GOTO") {
            std::string label;
            iss >> label;
            riscvCode.push_back("j " + label);
        } else if (token == "LABEL") {
            std::string label;
            iss >> label;
            riscvCode.push_back(label);
        } else if (token == "DEC") {
            std::string var, num;
            iss >> var >> num;
            //riscvCode.push_back("li a0, " + num.substr(1));
            //riscvCode.push_back("jal sbrk");
            if (stackOffset.find(var) == stackOffset.end() && var[0] != '*') {
                std::string temp;
                temp = var.substr(1);
                stackOffset[var] = 4 * stoi(temp);
                reg_cn = stoi(temp);
                tempqueue.push_back(stoi(temp));
                //offsetCounter -= 4;
            }
            riscvCode.push_back("sw s2, " + std::to_string(stackOffset[var]) + "(gp)");
            riscvCode.push_back("addi s2, s2, " + std::to_string(stoi(num.substr(1))));
        } else{
            std::string lhs = token;
            std::string rhs;
            std::getline(iss, rhs);
            rhs = rhs.substr(1);  // Remove leading gpace

            if (stackOffset.find(lhs) == stackOffset.end() && lhs[0] != '*') {
                std::string temp;
                temp = lhs.substr(1);
                stackOffset[lhs] = 4 * stoi(temp);
                tempqueue.push_back(stoi(temp));
                //offsetCounter -= 4;
            }

            if (rhs.find('+') != std::string::npos) {
                std::string eq, a, b;
                std::istringstream rhsIss(rhs);
                rhsIss >> eq >> a >> token >> b;
                riscvCode.push_back("lw t0, " + std::to_string(stackOffset[a]) + "(gp)");
                if (b[0] == '#') {
                    riscvCode.push_back("addi t1, t0, " + b.substr(1));
                } else {
                    riscvCode.push_back("lw t1, " + std::to_string(stackOffset[b]) + "(gp)");
                    riscvCode.push_back("add t1, t0, t1");
                }
                riscvCode.push_back("sw t1, " + std::to_string(stackOffset[lhs]) + "(gp)");
            } else if (lhs.find('*') != std::string::npos){
                std::string eq, var;
                std::istringstream rhsIss(rhs);
                rhsIss >> eq >> var;
                riscvCode.push_back("lw t5, " + std::to_string(stackOffset[var]) + "(gp)");
                riscvCode.push_back("lw t6, " + std::to_string(stackOffset[lhs.substr(1)]) + "(gp)");
                riscvCode.push_back("sw t5, 0(t6)");
            } else if (rhs.find('-') != std::string::npos) {
                std::string a, b, eq;
                std::istringstream rhsIss(rhs);
                rhsIss >> eq >> a >> token >> b;
                if(a[0] == '-'){
                    riscvCode.push_back("lw t0, " + std::to_string(stackOffset[token]) + "(gp)");
                    riscvCode.push_back("sub t0, x0, t0");
                    riscvCode.push_back("sw t0, " + std::to_string(stackOffset[lhs]) + "(gp)");
                }   
                else{
                    riscvCode.push_back("lw t0, " + std::to_string(stackOffset[a]) + "(gp)");
                    if (b[0] == '#') {
                        riscvCode.push_back("addi t1, t0, -" + b.substr(1));
                    } else {
                        riscvCode.push_back("lw t1, " + std::to_string(stackOffset[b]) + "(gp)");
                        riscvCode.push_back("sub t1, t0, t1");
                    }
                    riscvCode.push_back("sw t1, " + std::to_string(stackOffset[lhs]) + "(gp)");
                }
            } else if (rhs.find('*') != std::string::npos) {
                std::string eq, a, b;
                std::istringstream rhsIss(rhs);
                rhsIss >> eq >> a >> token >> b;
                if(a[0] != '*'){
                    riscvCode.push_back("lw t0, " + std::to_string(stackOffset[a]) + "(gp)");
                    riscvCode.push_back("lw t1, " + std::to_string(stackOffset[b]) + "(gp)");
                    riscvCode.push_back("mul t1, t0, t1");
                    riscvCode.push_back("sw t1, " + std::to_string(stackOffset[lhs]) + "(gp)");
                }
                else{
                    riscvCode.push_back("lw t5, " + std::to_string(stackOffset[a.substr(1)]) + "(gp)");
                    riscvCode.push_back("lw t6, 0(t5)");
                    riscvCode.push_back("sw t6, " + std::to_string(stackOffset[lhs]) + "(gp)");
                }
            } else if (rhs.find('/') != std::string::npos) {
                std::string eq, a, b;
                std::istringstream rhsIss(rhs);
                rhsIss >> eq >> a >> token >> b;
                riscvCode.push_back("lw t0, " + std::to_string(stackOffset[a]) + "(gp)");
                riscvCode.push_back("lw t1, " + std::to_string(stackOffset[b]) + "(gp)");
                riscvCode.push_back("div t1, t0, t1");
                riscvCode.push_back("sw t1, " + std::to_string(stackOffset[lhs]) + "(gp)");
            } else if (rhs.find('%') != std::string::npos) {
                std::string eq, a, b;
                std::istringstream rhsIss(rhs);
                rhsIss >> eq >> a >> token >> b;
                riscvCode.push_back("lw t0, " + std::to_string(stackOffset[a]) + "(gp)");
                riscvCode.push_back("lw t1, " + std::to_string(stackOffset[b]) + "(gp)");
                riscvCode.push_back("rem t1, t0, t1");
                riscvCode.push_back("sw t1, " + std::to_string(stackOffset[lhs]) + "(gp)");
            } else if (rhs.find('#') != std::string::npos) {
                std::string eq, num;
                std::istringstream rhsIss(rhs);
                rhsIss >> eq >> num;
                riscvCode.push_back("li t0, " + num.substr(1));
                riscvCode.push_back("sw t0, " + std::to_string(stackOffset[lhs]) + "(gp)");
            } else if (rhs.find('C') != std::string::npos){
                std::string func, call;
                std::istringstream rhsIss(rhs);
                rhsIss >> call >> call >> func;
                if(func != "read" && func != "write"){
                    for(int i = 0; i < tempqueue.size(); i++){
                        riscvCode.push_back("lw t2, " + std::to_string(4*tempqueue[i]) + "(gp)");
                        riscvCode.push_back("sw t2, 0(s2) ");
                        riscvCode.push_back("addi s2, s2, 4");
                    }
                    riscvCode.push_back("jal " + func);
                    for(int i = tempqueue.size() - 1; i > 0; i--){
                        riscvCode.push_back("addi s2, s2, -4");
                        riscvCode.push_back("lw t2, 0(s2) ");
                        riscvCode.push_back("sw t2, " + std::to_string(4*tempqueue[i]) + "(gp)");
                    }
                }
                else riscvCode.push_back("jal " + func);
                riscvCode.push_back("sw a0, " + std::to_string(stackOffset[lhs]) + "(gp)");
                argcounter = 0;
            } else if (rhs.find('&') != std::string::npos){
                std::string eq, var;
                std::istringstream rhsIss(rhs);
                rhsIss >> eq >> var;
                riscvCode.push_back("la t0, " + var.substr(1));
                riscvCode.push_back("sw t0, " + std::to_string(stackOffset[lhs]) + "(gp)");
            }  else if (rhs.find('=') != std::string::npos){
                std::string eq, var;
                std::istringstream rhsIss(rhs);
                rhsIss >> eq >> var;
                if(var[0] == 't'){
                    riscvCode.push_back("lw t0, " + std::to_string(stackOffset[var]) + "(gp)");
                    riscvCode.push_back("sw t0, " + std::to_string(stackOffset[lhs]) + "(gp)");
                }
                else{
                    riscvCode.push_back("lw t0, " + std::to_string(4*paramsOffset[var]+8) + "(sp)");
                    riscvCode.push_back("sw t0, " + std::to_string(stackOffset[lhs]) + "(gp)");
                }
            }
        }
    }

    // Add the final data segment setup
    riscvCode.push_back(".data");
    riscvCode.push_back(".align 4");
    riscvCode.push_back("_stack_start:");
    riscvCode.push_back(".space 1145140 # Add a zero to align to 4 bytes");
    riscvCode.push_back("_stack_top:");
    riscvCode.push_back("# Compiled .data continues here");

    riscvCode.insert(riscvCode.end(), dataCode.begin(), dataCode.end());



    return riscvCode;
}

// Function to read intermediate code from a file
std::vector<std::string> readIntermediateCode(const std::string& filename) {
    std::ifstream infile(filename);
    std::vector<std::string> intermediateCode;
    std::string line;

    while (std::getline(infile, line)) {
        intermediateCode.push_back(line);
    }

    return intermediateCode;
}

// Function to write RISC-V assembly code to a file
void writeRISCVCode(const std::vector<std::string>& riscvCode, const std::string& filename) {
    std::ofstream outfile(filename);

    for (const auto& line : riscvCode) {
        outfile << line << std::endl;
    }
}
