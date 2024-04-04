
#include "gorila.h"
#include "hw3_output.hpp"
#include <assert.h>
#include "symbol_table_dinosaur.h"
#include "codeGen_chan.h"
extern Trex *trex;
extern codeGen_chan *codegen_chan;

bool is_id(Node* node);


using std::string;

int global_while_ctr = 0;

Node* is_bool(Node* node)
{
    string bool_val;
    Node* ret_node;
    Symbol *s;
    if (is_id(node) && node->type == "BOOL")
    {
        s = trex->search_symbol(node->value);
        if(!s){
            output::errorUndef(yylineno, node->value);
            exit(0);
        }
        bool_val = s->numerical_value == 1 ? "true" : "false";
    }
    else
    {
        bool_val = node->value;
    }
    if (node->type != "BOOL")
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    ret_node = new Node(bool_val, "BOOL", node->reg);
    //////my change
    ret_node->false_label = node->false_label;
    ret_node->true_label = node->true_label;
    ret_node->tmp_code_buffer = node->tmp_code_buffer;
    ////////

    return ret_node;
}



void is_num(Node* node)
{
    
    if (node->type != "INT" && node->type != "BYTE")
    {
        if(DEBUG){ std::cout << "this node is not a number: " + node->value + ", "  + node->type << std::endl;}
        output::errorMismatch(yylineno);
        exit(0);
    }
}

BOOL_CLASS* calc_relop(Node* left, Node* op, Node* right)
{
    int left_num, right_num;
    BOOL_CLASS* ret_node;
    bool res;
    try{
        if (is_id(left))
        {
            Symbol *s = trex->search_symbol(left->value);
            if (!s)
            {
                output::errorUndef(yylineno, left->value);
                exit(0);
            }
            if (s->type != "INT")
            left_num = s->numerical_value;
        }
        else{
            left_num = std::stoi(left->value);
        }
        if (is_id(right))
        {
            Symbol *s = trex->search_symbol(right->value);
            if (!s)
            {
                output::errorUndef(yylineno, right->value);
                exit(0);
            }
            left_num = s->numerical_value;
        }
        else {
            right_num = std::stoi(right->value);
        }
        if (op->value == "<"){
            res = left_num < right_num;
        }
        else if (op->value == ">"){
            res =  left_num > right_num;
        } 
        else if (op->value == "<="){
            res = left_num <= right_num;
        }
        else if (op->value == ">="){
            res = left_num >= right_num;
        } 
        else if (op->value == "=="){
            res = left_num == right_num;
        }
        else if (op->value == "!="){
            res = left_num != right_num;
        }
        else{
            output::errorMismatch(yylineno);
            exit(0);
        }
        
    } 
    catch (std::exception &e)
    {
        assert(false);
        output::errorMismatch(yylineno);
        exit(0);
    }
    if(res)
        ret_node = new BOOL_CLASS("true");
    else
        ret_node = new BOOL_CLASS("false");

    ret_node->true_label = codegen_chan->code_buffer.freshLabel();
    ret_node->false_label = codegen_chan->code_buffer.freshLabel();
    //ret_node->tmp_code_buffer = codegen_chan->gen_relop(ret_node, left, op->value, right);
    codegen_chan->gen_relop(ret_node, left, op->value, right);
    ret_node->tmp_code_buffer += "br i1 " + ret_node->reg +  ", label %" + ret_node->true_label + ", label %" + ret_node->false_label + '\n';
    return ret_node;
}


bool is_id(Node* node)
{
    try{
        ID_CLASS *id_chan = dynamic_cast<ID_CLASS *>(node);
        if(id_chan != NULL){
            if(DEBUG){ std::cout << "this node is id: " + node->value + ", "  + node->type << std::endl;}
            return true;}
        else{
            return false;
        }
    } catch (std::exception &e) {
        return false;
    }
}

bool is_id_num(Node* node)
{
    return is_id(node) && (node->type == "INT" || node->type == "BYTE");
}
// if return type is true - number
// if return type is false - var
Node* try_number_cast_type(Node* type, Node* cast_me_senpai) // can we cast int to int?
{
    if (type->value !="int" && type->value != "byte")
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (!(cast_me_senpai->type != "INT" && cast_me_senpai->type != "BYTE"))
    {
        if (!(is_id_num(cast_me_senpai)))
        {
            return new Node(cast_me_senpai->value, type->type, cast_me_senpai->reg);
        } else {
            // search for its value in symbol table, and return a new node of type "type.value" and value from symbo table
            Symbol* s = trex->search_symbol(cast_me_senpai->value);
            if(s == NULL){
                output::errorUndef(yylineno, cast_me_senpai->value);
                exit(0);
            } else {
                return new Node(std::to_string(s->numerical_value), type->type, cast_me_senpai->reg);
            }
        }
    } else {
        output::errorMismatch(yylineno);
        exit(0);
    }
}



void is_byte(Node* node)
{
    try
    {
        unsigned int byte_num = std::stoi(node->value);
        if (byte_num > 255)
        {
            output::errorByteTooLarge(yylineno, node->value);
            exit(0);
        }
    }
    catch (std::exception &e)
    {
        assert(false);
        output::errorMismatch(yylineno);
        exit(0);
    }
}

Node* call_function(Node* func, Node* argument) {
    if(DEBUG){ std::cout << "entere call_functionr: " + func->value;}
    Func* current_function = trex->search_func(func->value);
    Node* ret_node;
    bool node_is_id = false;
    if (current_function == NULL){
        output::errorUndefFunc(yylineno, func->value);
        exit(0);
    }
    if (is_id(argument))
    {
        Symbol *s = trex->search_symbol(argument->value);
        if(!s) {
            output::errorUndef(yylineno, argument->value);
            exit(0);
        }
    }
    ret_node = new Node(func->value, "INT");
    codegen_chan->gen_call_function(func, argument, ret_node);
    if(current_function->argument_type == argument->type){
        if(current_function->output_type != "VOID"){
            return ret_node;
        }
        return NULL;
    } else if(current_function->argument_type == "INT" && argument->type == "BYTE"){
        if(current_function->output_type != "VOID"){
            return ret_node;
        }
        return NULL;
    }
    output::errorPrototypeMismatch(yylineno, func->value, current_function->argument_type);
    exit(0);
}

void add_symbol(Node* type, Node* new_symbol, Node* assigned_node){
    int new_symbol_numerical_value = 1;
    bool is_assigned_node_id = false;
    Symbol *s;
    if (assigned_node)
    {
        if (!((type->type == assigned_node->type) || (type->type == "INT" && assigned_node->type == "BYTE")))
        {
            if(DEBUG){ std::cout << "add_symbol error case 1" << std::endl;}
            output::errorMismatch(yylineno);
            exit(0);
        } 
        if (type->type == "STRING"|| assigned_node->type == "STRING" || type->type == "VOID"|| assigned_node->type == "VOID")
        {
            if(DEBUG){ std::cout << "add_symbol error case 2" << std::endl;}
            output::errorMismatch(yylineno);
            exit(0);
        }
        if(is_id(assigned_node))
        {
            s = trex->search_symbol(assigned_node->value); // int a = b;
            if(s)
            {
                new_symbol_numerical_value = s->numerical_value;
                is_assigned_node_id = true;
            }
            else{
                if(DEBUG){ std::cout << "add_symbol error case 3" << std::endl;}
                output::errorUndef(yylineno, assigned_node->value);
                exit(0);
            }
        }
        else // int a = 5; // bool a = true;
        {
            if (assigned_node->type == "BOOL")
            {
                int val = assigned_node->value == "true" ? 1 : 0;
                string str_val = assigned_node->value == "true" ? "1" : "0";
                new_symbol_numerical_value = val;
                if(assigned_node->reg == "") // in case it was assignment of this: a > b and c > d 
                {
                    codegen_chan->code_buffer.emit(assigned_node->tmp_code_buffer);
                    string end_label = codegen_chan->code_buffer.freshLabel();
                    codegen_chan->code_buffer.emit(assigned_node->true_label + ":");
                    codegen_chan->store_bool_stack(s->offset, 0);
                    codegen_chan->code_buffer.emit("br label %" + end_label);
                    codegen_chan->code_buffer.emit(assigned_node->false_label + ":");
                    codegen_chan->code_buffer.emit("br label %" + end_label);
                    codegen_chan->code_buffer.emit(end_label + ":");
                    trex->add_symbol(new_symbol->value, type->type, new_symbol_numerical_value);
                    new_symbol->type = type->type;
                    return;
                }
            }
            else {
                if (assigned_node->reg == "")
                {
                    try{
                        new_symbol_numerical_value = std::stoi(assigned_node->value);
                    }
                    catch (std::exception& e) {
                        if(DEBUG){ std::cout << "add_symbol error case 4" << std::endl;}
                        output::errorMismatch(yylineno);
                        exit(0);
                    }
                }
            }
        }
    }
    trex->add_symbol(new_symbol->value, type->type, new_symbol_numerical_value);
    new_symbol->type = type->type;
    Symbol *new_s = trex->search_symbol(new_symbol->value);
    if(assigned_node)
        codegen_chan->store_to_stack(new_s->offset, assigned_node);
    else // not really bool but we need to assign zero for uninitalized vars
        codegen_chan->store_bool_stack(new_s->offset, 0);
}




void assign_symbol(Node* existing_symbol, Node* assigned_node)
{
    int assigned_node_numerical_value = 1;
    Symbol *s = trex->search_symbol(existing_symbol->value);
    if(!s)
    {
        output::errorUndef(yylineno, existing_symbol->value);
        exit(0);
    }
    if (!((s->type == assigned_node->type) || (s->type == "INT" && assigned_node->type == "BYTE")))
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (existing_symbol->type == "VOID" || assigned_node->type=="VOID")
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if(is_id(assigned_node)){
        Symbol *assigned_s = trex->search_symbol(assigned_node->value);
        if(!assigned_s)
        {
            output::errorUndef(yylineno, existing_symbol->value);
            exit(0);
        } else{
            assigned_node_numerical_value = assigned_s->numerical_value;
            if(assigned_node->type == "BOOL")
            {
                s->save_bool_atr(assigned_s);
            }
        }
    } else {
        try
        {
            if (assigned_node->type == "BOOL")
            {
                int val = assigned_node->value == "true" ? 1 : 0;
                string str_val = assigned_node->value == "true" ? "1" : "0";
                assigned_node_numerical_value = val;
                if(assigned_node->reg == "") // in case it was assignment of this: a > b and c > d 
                {
                    codegen_chan->code_buffer.emit(assigned_node->tmp_code_buffer);
                    string end_label = codegen_chan->code_buffer.freshLabel();
                    codegen_chan->code_buffer.emit(assigned_node->true_label + ":");
                    codegen_chan->store_bool_stack(s->offset, 1);
                    codegen_chan->code_buffer.emit("br label %" + end_label);
                    codegen_chan->code_buffer.emit(assigned_node->false_label + ":");
                    codegen_chan->store_bool_stack(s->offset, 0);
                    codegen_chan->code_buffer.emit("br label %" + end_label);
                    codegen_chan->code_buffer.emit(end_label + ":");
                    return;
                }
                s->save_bool_atr(assigned_node->tmp_code_buffer, assigned_node->true_label, assigned_node->false_label);
            }
            else{ //case byte or int or func
                if (assigned_node->reg == "") // it is literal. function alraeady have reg
                {
                    assigned_node_numerical_value = std::stoi(assigned_node->value);
                    codegen_chan->assign_reg_for_literal(assigned_node, assigned_node->value);
                }
            }
        }
        catch (std::exception& e) {
            output::errorMismatch(yylineno);
            exit(0);
        }
    }
    codegen_chan->store_to_stack(s->offset, assigned_node);
}


Node* if_else_statment(Node* condition, Node* case_true, Node* case_false = new Node("void", "VOID"))
{
    return (condition->value == "true"? case_true : case_false);
}

void add_scope()
{
    trex->add_scope();
}

void remove_scope()
{
	trex->remove_scope();
}

ID_CLASS* search_and_return_id(Node* node)
{
    Symbol* s = trex->search_symbol(node->value);
    if(!s)
    {
        output::errorUndef(yylineno, node->value);
        exit(0);
    }
    ID_CLASS *ret_node = new ID_CLASS(node->value, s->type);
    codegen_chan->load_from_stack(ret_node, s->offset);
    if(s->type == "BOOL")
        ret_node->true_label = codegen_chan->code_buffer.freshLabel();
        ret_node->false_label = codegen_chan->code_buffer.freshLabel();
        ret_node->tmp_code_buffer = "br i1 " + ret_node->reg +  ", label %" + ret_node->true_label + ", label %" + ret_node->false_label + '\n';
        // s->copy_bool_atr(ret_node);
    return ret_node;
}

Node* plus_minus_mult_divide(Node *left, Node* op, Node *right)
{
    int left_operand_numerical_val, right_operand_numerical_val;

    if(is_id_num(left)){
        Symbol *left_s = trex->search_symbol(left->value);
        if(!left_s){
            output::errorUndef(yylineno, left->value);
            exit(0);
        }
        left_operand_numerical_val = left_s->numerical_value;
    } else {
        if(left->type == "INT" || left->type == "BYTE"){
            left_operand_numerical_val = std::stoi(left->value);
        } else {
            output::errorMismatch(yylineno);
            exit(0);
        }
    }

    if(is_id_num(right)){
        Symbol *right_s = trex->search_symbol(right->value);
        if(!right_s){
            output::errorUndef(yylineno, right->value);
            exit(0);
        }
        right_operand_numerical_val = right_s->numerical_value;
    } else {
        if(right->type == "INT" || right->type == "BYTE"){
            right_operand_numerical_val = std::stoi(right->value);
        } else {
            output::errorMismatch(yylineno);
            exit(0);
        }
    }
    int result_numerical_value;
    if(op->value == "+"){
        result_numerical_value = left_operand_numerical_val + right_operand_numerical_val;
    } else if(op->value == "-"){
        result_numerical_value = left_operand_numerical_val - right_operand_numerical_val;
    } else if(op->value == "*"){
        result_numerical_value = left_operand_numerical_val * right_operand_numerical_val;
    } else if(op->value == "/"){
        result_numerical_value = left_operand_numerical_val / 1;
    } else {
        assert(false);
    }
    Node *result_node;
    if(right->type == "INT" || left->type == "INT"){
        result_node = new Node(std::to_string(result_numerical_value), "INT");
    } else {
        result_node = new Node(std::to_string(result_numerical_value), "BYTE");
    }
    codegen_chan->gen_binop(result_node, left, op->value, right);
    return result_node;
}

NUMB *CREATE_EXP_OF_NUMB(Node *node)
{
    NUMB* ret_node = new NUMB(node);
    codegen_chan->assign_reg_for_literal(ret_node, ret_node->value);
    return ret_node;
}


NUM_CLASS *CREATE_EXP_OF_NUM(Node *node)
{
    NUM_CLASS *ret_node = new NUM_CLASS(node);
    codegen_chan->assign_reg_for_literal(ret_node, node->value);
    return ret_node;
}
/*
BOOL_CLASS *evaluate_and_exp(Node* node1, Node *node2)
{
    BOOL_CLASS *ret_node = new BOOL_CLASS("true");
    ret_node->continue_label = codegen_chan->code_buffer.freshLabel();
    ret_node->continue_buffer = ret_node->continue_label + ":" + '\n';
    ret_node->false_label = codegen_chan->code_buffer.freshLabel();
    ret_node->false_buffer = ret_node->false_label + ":" + '\n';
    ret_node->true_label = codegen_chan->code_buffer.freshLabel();
    ret_node->true_buffer = ret_node->true_label + ":" + '\n';
    if (node1->reg != "")
    {
        codegen_chan->check_first_bool_reg_and_exp(node1->reg, ret_node->start_buffer, ret_node->continue_label, ret_node->false_label);
    } else {
        codegen_chan->add_and_exp_br_first_exp(node1->true_buffer, node1->false_buffer, ret_node->continue_label, ret_node->false_label);
    }
    if (node2->reg != "")
    {
        codegen_chan->check_second_bool_reg_and_exp(node2->reg, ret_node->continue_buffer, ret_node->true_label, ret_node->false_label);
    } else {
        codegen_chan->add_sub_expression_code_in_continue(node2->true_buffer, node2->false_buffer, ret_node->true_label, ret_node->false_label);
    }
}
*/

BOOL_CLASS *create_bool_class_from_literal(string str)
{
    string reg_val = (str == "true"? "1" : "0");
    BOOL_CLASS *ret_node = new BOOL_CLASS(str);
    ret_node->true_label = codegen_chan->code_buffer.freshLabel();
    ret_node->false_label = codegen_chan->code_buffer.freshLabel();
    codegen_chan->assign_reg_for_bool_literal(ret_node, reg_val);
    ret_node->tmp_code_buffer = "br i1 " + ret_node->reg +  ", label %" + ret_node->true_label + ", label %" + ret_node->false_label + '\n';
    return ret_node;
}

BOOL_CLASS *evaluate_or_exp(Node* node1, Node *node2)
{
    BOOL_CLASS *ret_node = new BOOL_CLASS("true");
    ret_node->true_label = node2->true_label;
    ret_node->false_label = node2->false_label;
    ret_node->tmp_code_buffer = node1->tmp_code_buffer + node1->false_label + ":\n"
                                + node2->tmp_code_buffer + node1->true_label + ":\n" 
                                + "br label %" + ret_node->true_label + "\n";
    return ret_node;
}

BOOL_CLASS *evaluate_and_exp(Node* node1, Node *node2)
{
    BOOL_CLASS *ret_node = new BOOL_CLASS("true");
    ret_node->true_label = node2->true_label;
    ret_node->false_label = node2->false_label;
    ret_node->tmp_code_buffer = node1->tmp_code_buffer + node1->true_label + ":\n"
                                + node2->tmp_code_buffer + node1->false_label + ":\n" 
                                + "br label %" + ret_node->false_label + "\n";
    return ret_node;
}

BOOL_CLASS *evaluate_not_exp(Node *bool_exp)
{
    BOOL_CLASS *ret_node = new BOOL_CLASS("true");
    ret_node->true_label = bool_exp->false_label;
    ret_node->false_label = bool_exp->true_label;
    ret_node->tmp_code_buffer = bool_exp->tmp_code_buffer;
    return ret_node;
}

STRING_CLASS *add_string_literal(Node *node)
{
    STRING_CLASS *ret_node = new STRING_CLASS(node);
    codegen_chan->add_global_string(ret_node);
    return ret_node;
}