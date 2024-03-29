
#include "gorila.h"
#include "hw3_output.hpp"
#include <assert.h>
#include "symbol_table_dinosaur.h"
extern Trex *trex;
bool is_id(Node* node);


using std::string;

int global_while_ctr = 0;

Node* is_bool(Node* node)
{
    string bool_val;
    if (is_id(node) && node->type == "BOOL")
    {
        Symbol *s = trex->search_symbol(node->value);
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
    return new Node(bool_val, "BOOL");
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

bool calc_relop(Node* left, Node* op, Node* right)
{
    int left_num, right_num;
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
            return left_num < right_num;
        } 
        else if (op->value == ">"){
            return left_num > right_num;
        } 
        else if (op->value == "<="){
            return left_num <= right_num;
        }
        else if (op->value == ">="){
            return left_num >= right_num;
        } 
        else if (op->value == "=="){
            return left_num == right_num;
        }
        else if (op->value == "!="){
            return left_num != right_num;
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
            return new Node(cast_me_senpai->value, type->type);
        } else {
            // search for its value in symbol table, and return a new node of type "type.value" and value from symbo table
            Symbol* s = trex->search_symbol(cast_me_senpai->value);
            if(s == NULL){
                output::errorUndef(yylineno, cast_me_senpai->value);
                exit(0);
            } else {
                return new Node(std::to_string(s->numerical_value), type->type);
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
    if (current_function == NULL){
        output::errorUndefFunc(yylineno, func->value);
        exit(0);
    } else if(current_function->argument_type == argument->type){
        if(current_function->output_type != "VOID"){
            return new Node(argument->value, "INT");
        }
        return NULL;
    } else if(current_function->argument_type == "INT" && argument->type == "BYTE"){
        if(current_function->output_type != "VOID"){
            return new Node(argument->value, "INT");
        }
        return NULL;
    }
    output::errorPrototypeMismatch(yylineno, func->value, current_function->argument_type);
    exit(0);

}

void add_symbol(Node* type, Node* new_symbol, Node* assigned_node){
    int new_symbol_numerical_value = 1;
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
            Symbol *s = trex->search_symbol(assigned_node->value); // int a = b;
            if(s)
            {
                new_symbol_numerical_value = s->numerical_value;
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
                new_symbol_numerical_value = val;
            }
            else {
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
    trex->add_symbol(new_symbol->value, type->type, new_symbol_numerical_value);
    new_symbol->type = type->type;
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
        }
    } else {
        try
        {
            if (assigned_node->type == "BOOL")
            {
                int val = assigned_node->value == "true" ? 1 : 0;
                assigned_node_numerical_value = val;
            }
            else{
                assigned_node_numerical_value = std::stoi(assigned_node->value);
            }
        }
        catch (std::exception& e) {
            output::errorMismatch(yylineno);
            exit(0);
        }
    } 
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
    return new ID_CLASS(node->value, s->type);
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
        codegen_chan->gen_binop();
        result_numerical_value = left_operand_numerical_val / right_operand_numerical_val;
    } else {
        assert(false);
    }

    if(right->type == "INT" || left->type == "INT"){
        return new Node(std::to_string(result_numerical_value), "INT");
    } else {
        return new Node(std::to_string(result_numerical_value), "BYTE");
    }
}