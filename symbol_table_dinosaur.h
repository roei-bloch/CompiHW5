#ifndef SYMBOL_TABLE_DINOZAUR
#define SYMBOL_TABLE_DINOZAUR

#include <string>
#include <vector>
#include <iostream>
#include "codeGen_chan.h"

using std::string;
#define NUM_OF_FUNCS 3
#define DEBUG 0
extern codeGen_chan *codegen_chan;

class Symbol{
public:
    string name;
    int offset;
    string type;
    int numerical_value;

    Symbol(string s_name, int s_offset, string s_type, int s_numerical_value = 1): name(s_name), offset(s_offset), type(s_type), numerical_value(s_numerical_value){} 
};

class Func{
public:
    string name;
    string argument_type;
    string output_type;
    int offset;

    Func(string f_name, string f_argument_type, string f_output_type, int f_offset = 0): 
        name(f_name), argument_type(f_argument_type), output_type(f_output_type), offset(f_offset){}
};

/****************************  SCOPE  ************************************/

class Scope{
public:
    std::vector<Symbol*> symbols;
    int current_offset;

    Scope(int s_offset){
        current_offset = s_offset;
    }
    void print_scope_content(){
        for(Symbol* s : symbols){
            std::cout << s->name + " " << s->type + " " << s->offset << std::endl;
        }
    }

    void clear_scope_content(){
        for(Symbol* s : symbols) {
            codegen_chan->clean_offset(s->offset);
        }
    }

    void add_symbol(string name, string type, int numerical_value = 1){
        symbols.push_back(new Symbol(name, current_offset++, type, numerical_value));
    }

    //return symbol  if symbol exists, "null" else
    Symbol* search_symbol_in_scope(string symbol_name){
        for(Symbol* s : symbols){
            if(s->name == symbol_name){
                return s;
            }
        }
        return NULL;
    }

    
};


/****************************  TREX  ************************************/

class Trex{
public:
    std::vector<Scope*> scopes;
    std::vector<Func*> funcs;

    Trex(){
        funcs.push_back(new Func("print", "STRING", "VOID"));
        funcs.push_back(new Func("printi", "INT", "VOID"));
        funcs.push_back(new Func("readi", "INT", "INT"));
        scopes.push_back(new Scope(0));
    }

    void add_scope()
    {
        int offset = scopes.back()->current_offset;
        scopes.push_back(new Scope(offset));
    }

    void remove_scope()
    {
        std::cout << "---end scope---" << std::endl;
        if(scopes.size() > 1){
            scopes.back()->print_scope_content();
            scopes.pop_back();
        } else {
            for(int i = 0; i < NUM_OF_FUNCS; i++){
                Func *func = funcs[i];
                std::cout << func->name + " (" + func->argument_type + ")->" + func->output_type + " 0" << std::endl; 
            }
            scopes.back()->print_scope_content();
            scopes.pop_back();
        }

    }


    //return symbol type if symbol exists, "null" else
    Symbol* search_symbol(string symbol_name){
        for(int i = scopes.size() - 1; i >= 0; i--){
            Symbol* s = scopes[i]->search_symbol_in_scope(symbol_name);
            if(s != NULL){
                return s;
            }
        }
        return NULL;
    }

    //return func if symbol exists, "null" else
    Func* search_func(string func_name){
        for(Func* f : funcs){
            if(f->name == func_name){
                return f;
            }
        }
        return NULL;
    }

    void add_symbol(string name, string type, int numerical_value = 1)
    {
        if (!(search_symbol(name)) && (!(search_func(name))))
        {
            this->scopes.back()->add_symbol(name, type, numerical_value);
            return;
        }
        output::errorDef(yylineno, name);
        exit(0);
    }

};




#endif  //SYMBOL_TABLE_DINOZAUR