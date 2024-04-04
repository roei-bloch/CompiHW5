#ifndef GORILA_H
#define GORILA_H

#include <string>
#include <iostream>



//#include "codeGen_chan.h"

using std::string;
extern int yylineno;
extern int global_while_ctr;
#define YYSTYPE Node*
#ifndef DEBUG
#define DEBUG 0
#endif


class Node{
public:
    string reg;
    string value; // The actual text (lexema)
    string type; // The inheriting class
    string tmp_code_buffer;
    string false_label;
    string true_label;
    Node(string val) : value(val), type(""), reg(""), tmp_code_buffer(""), false_label(""), true_label("") {}
    Node(string val, string type): value(val), type(type), reg(""), tmp_code_buffer(""), false_label(""), true_label("") {}
    Node(string val, string type, string reg): value(val), type(type), reg(reg), tmp_code_buffer(""), false_label(""), true_label("") {}
    string get_value() {return value;}
    void set_value(string str) {this->value = str;}
    string get_type() {return type;}
    void set_type(string str) {this->type = str;}
    virtual ~Node() {}
    private:
};

Node* is_bool(Node* node);
void is_num(Node* node);
bool cast_type(Node* node1, Node* node2);
void is_byte(Node* node);

class Type_CLASS : public Node{
public:
    Type_CLASS(Node* node, string type) : Node(node->value, type){}
};

class ID_CLASS : public Node {
public:
    ID_CLASS(Node* node) : Node(node->value, "") {}
    ID_CLASS(string value, string type) : Node(value, type) {}
    ID_CLASS(string value, string , string reg) : Node(value, type, reg) {}
};

class NUM_CLASS : public Node{
public:

    NUM_CLASS(Node* node) : Node(node->value, "INT", node->reg)
    {
        // if(DEBUG){
        //     std::cout << "ctor NUM_CLASS, val = " + node->value << std::endl; 
        // }
    }

    NUM_CLASS(int numerical_value) : Node(std::to_string(numerical_value), "INT"){}
};

class NUMB : public Node{
public:

    NUMB(Node* node) : Node(node->value, "BYTE", node->reg){}
};

class STRING_CLASS : public Node{
public:

    STRING_CLASS(Node* node) : Node(node->value, "STRING", node->reg){}
};

class BOOL_CLASS : public Node{
public:

    BOOL_CLASS(Node* node) : Node(node->value, "BOOL", node->reg){}
    BOOL_CLASS(string val) : Node(val, "BOOL"){}
};

class EXP : public Node{
public:
    string continue_label;
    string end_label;
    EXP(Node* node) : Node(node->value, "EXP"){}
};

void remove_scope();
void add_scope();
void add_symbol(Node* type, Node* new_symbol, Node* assigned_node = NULL);
void assign_symbol(Node* existing_symbol, Node* assigned_node);
Node* call_function(Node* func, Node* argument);
Node* try_number_cast_type(Node* type, Node* cast_me_senpai);
ID_CLASS* search_and_return_id(Node* node);
Node* plus_minus_mult_divide(Node *left, Node* op, Node *right);
NUM_CLASS *CREATE_EXP_OF_NUM(Node *node);
NUMB *CREATE_EXP_OF_NUMB(Node * node);
BOOL_CLASS *evaluate_and_exp(Node* node1, Node* node2);
BOOL_CLASS *calc_relop(Node* left, Node* op, Node* right);
BOOL_CLASS *create_bool_class_from_literal(string str);
BOOL_CLASS *evaluate_not_exp(Node *bool_exp);
BOOL_CLASS *evaluate_or_exp(Node* node1, Node *node2);
STRING_CLASS *add_string_literal(Node *node);
#endif /* GORILA_H */