#ifndef CODEGEN_CHAN
#define CODEGEN_CHAN

#include "bp.hpp"
#include "gorila.h"
#include <string>

using std::string;

class codeGen_chan{
    int max_reg_index;
    CodeBuffer code_buffer;
    codeGen_chan();
    void emit_globals();
    string allocate_reg();
    void gen_binop(EXP *result, EXP *left, string op, EXP *right);
    void emit_open_main_chan();
    void clean_offset(int offset);
};









#endif // CODEGEN_CHAN