#ifndef CODEGEN_CHAN
#define CODEGEN_CHAN

#include "bp.hpp"
#include "gorila.h"
#include "symbol_table_dinosaur.h"
#include <string>

using std::string;

class codeGen_chan{
    int max_reg_index;
    CodeBuffer code_buffer;
    codeGen_chan();
    void emit_globals();
    string allocate_reg();
    void gen_binop(EXP *result, EXP *left, string op_str, EXP *right);
    void gen_relop(EXP *result, EXP *left, string op_str, EXP *right);
    void emit_open_main_chan();
    void clean_offset(int offset);
    void load_from_stack(Node* id_node,int offset);
    void assign_reg_for_literal(Node* num_node, string val);
    void store_to_stack(int offset, Node *assigned_node);
};













#endif // CODEGEN_CHAN