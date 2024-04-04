#ifndef CODEGEN_CHAN
#define CODEGEN_CHAN


#include "bp.hpp"
#include "gorila.h"
// #include "symbol_table_dinosaur.h"
#include <string>
#include <utility>

using std::string;

class codeGen_chan{
public:
    std::vector<std::pair<string, string>> while_labels_list_chan;
    std::vector<std::pair<string, string>> if_labels_list_chan;
    int max_reg_index;
    CodeBuffer code_buffer;
    codeGen_chan();
    void emit_globals();
    string allocate_reg();
    void gen_binop(Node *result, Node *left, string op_str, Node *right);
    string gen_relop(Node *result, Node *left, string op_str, Node *right);
    void emit_open_main_chan();
    void clean_offset(int offset);
    void load_from_stack(Node* id_node,int offset);
    void assign_reg_for_literal(Node* num_node, string val);
    void store_to_stack(int offset, Node *assigned_node);
    void gen_call_function(Node *function, Node *argument, Node *ret_node);
    void add_while_labels(Node* bool_exp);
    void add_if_labels(Node *bool_exp);
    void add_break();
    void add_continue();
    void end_while_actions();
    void end_if_actions();
    void add_closing_scope();
    void skip_else();
    void end_else_actions();
    void add_return();
    void assign_reg_for_bool_literal(Node* num_node, string val);
    void add_global_string(Node* node);
    void pre_while();
    //void check_first_bool_reg_and_exp(string bool_reg, string start_buffer, string continue_label, string false_label);
    //void check_second_bool_reg_and_exp(string bool_reg, string continue_buffer, string true_label, string false_label);
    //void add_and_exp_br(string true_buffer, string false_buffer, string continue_label, string false_label);
    //void add_sub_expression_code_in_continue(string continue_buffer, string sub_start_buffer, string sub_continue_buffer, string sub_false_buffer, string sub_true_buffer);
};













#endif // CODEGEN_CHAN