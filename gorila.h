#ifndef GORILA_H
#define GORILA_H

#include <string>
#include <iostream>
#include "codeGen_chan.h"

using std::string;
extern int yylineno;
extern int global_while_ctr;
extern codeGen_chan *codegen_chan;
#define YYSTYPE Node*
#ifndef DEBUG
#define DEBUG 0
#endif


class Node{
public:
    string reg;
    string value; // The actual text (lexema)
    string type; // The inheriting class
    Node(string val) : value(val), type(""), reg(""){}
    Node(string val, string type): value(val), type(type), reg("") {}
    Node(string val, string type, string reg): value(val), type(type), reg(reg) {}
    string get_value() {return value;}
    void set_value(string str) {this->value = str;}
    string get_type() {return type;}
    void set_type(string str) {this->type = str;}
    virtual ~Node() {}
    private:
};

Node* is_bool(Node* node);
void is_num(Node* node);
bool calc_relop(Node* left, Node* op, Node* right);
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
};

class NUM_CLASS : public Node{
public:

    NUM_CLASS(Node* node) : Node(node->value, "INT")
    {
        // if(DEBUG){
        //     std::cout << "ctor NUM_CLASS, val = " + node->value << std::endl; 
        // }
    }

    NUM_CLASS(int numerical_value) : Node(std::to_string(numerical_value), "INT"){}
};

class NUMB : public Node{
public:

    NUMB(Node* node) : Node(node->value, "BYTE"){}
};

class STRING_CLASS : public Node{
public:

    STRING_CLASS(Node* node) : Node(node->value, "STRING"){}
};

class BOOL_CLASS : public Node{
public:

    BOOL_CLASS(Node* node) : Node(node->value, "BOOL"){}
    BOOL_CLASS(string val) : Node(val, "BOOL"){}
};

class EXP : public Node{
public:


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


#endif /* GORILA_H */