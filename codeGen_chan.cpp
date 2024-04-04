#include "codeGen_chan.h"
#include "bp.hpp"

int str_index = 0;

codeGen_chan::codeGen_chan()
{
    max_reg_index = 0;
}

void codeGen_chan::emit_globals()
{
    code_buffer.emitGlobal("declare i32 @scanf(i8*, ...)");
    code_buffer.emitGlobal("declare i32 @printf(i8*, ...)");
    code_buffer.emitGlobal("declare void @exit(i32)");
    code_buffer.emitGlobal("@.int_specifier_scan = constant [3 x i8] c\"%d\\00\"");
    code_buffer.emitGlobal("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"");
    code_buffer.emitGlobal("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"");
    code_buffer.emitGlobal("@.invalid_div_msg = constant [23 x i8] c\"Error division by zero\\00\"");
    code_buffer.emitGlobal("");
    code_buffer.emitGlobal("define i32 @readi(i32) {");
    code_buffer.emitGlobal("    %ret_val = alloca i32");
    code_buffer.emitGlobal("    %spec_ptr = getelementptr [3 x i8], [3 x i8]* @.int_specifier_scan, i32 0, i32 0");
    code_buffer.emitGlobal("    call i32 (i8*, ...) @scanf(i8* %spec_ptr, i32* %ret_val)");
    code_buffer.emitGlobal("    %val = load i32, i32* %ret_val");
    code_buffer.emitGlobal("    ret i32 %val");
    code_buffer.emitGlobal("}");
    code_buffer.emitGlobal("");
    code_buffer.emitGlobal("define void @printi(i32) {");
    code_buffer.emitGlobal("    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0");
    code_buffer.emitGlobal("    call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)");
    code_buffer.emitGlobal("    ret void");
    code_buffer.emitGlobal("}");
    code_buffer.emitGlobal("");
    code_buffer.emitGlobal("define void @print(i8*) {");
    code_buffer.emitGlobal("    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0");
    code_buffer.emitGlobal("    call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)");
    code_buffer.emitGlobal("    ret void");
    code_buffer.emitGlobal("}");
    code_buffer.emitGlobal("define void @validatedevision(i32) {");
    code_buffer.emitGlobal("    %cond = icmp eq i32 %0, 0");
    code_buffer.emitGlobal("    br i1 %cond, label %Invalid_div, label %Valid_div");
    code_buffer.emitGlobal("Invalid_div:");
    code_buffer.emitGlobal("    %msg_ptr = getelementptr [23 x i8], [23 x i8]* @.invalid_div_msg, i32 0, i32 0");
    code_buffer.emitGlobal("    call void (i8*) @print(i8* %msg_ptr)");
    code_buffer.emitGlobal("    call void (i32) @exit(i32 0)");
    code_buffer.emitGlobal("    ret void");
    code_buffer.emitGlobal("Valid_div:");
    code_buffer.emitGlobal("    ret void");
    code_buffer.emitGlobal("}");
}



string codeGen_chan::allocate_reg(){
    string new_reg = "%reg" + to_string(max_reg_index++);
    return new_reg;
}



void codeGen_chan::emit_open_main_chan()
{
    code_buffer.emit("define i32 @main() {");
    code_buffer.emit("%arr_size = add i32 50, 0");
    code_buffer.emit("%frame_chan = alloca i32, i32 %arr_size");
}

void codeGen_chan::clean_offset(int offset)
{
    string reg = allocate_reg();
    code_buffer.emit(reg + " = getelementptr i32, i32* %frame_chan, i32 " + std::to_string(offset));
    code_buffer.emit("store i32 0, i32* " + reg);
}



void codeGen_chan::gen_binop(Node *result, Node *left, string op_str, Node *right){
    string llvm_op;
    if(op_str == "/"){
        code_buffer.emit("call void @validatedevision(i32 " + right->reg + ")");
        if(result->type == "INT"){
            llvm_op = "sdiv";
        } else {
            llvm_op = "udiv";
        }
    } else if(op_str == "*"){
        llvm_op = "mul";
    } else if(op_str == "+"){
        llvm_op = "add";
    } else {
        llvm_op = "sub";
    }
    result->reg = allocate_reg();
    code_buffer.emit(result->reg + " = " + llvm_op + " i32 " + left->reg + ", " + right->reg);
    if(result->type == "BYTE"){
        string new_reg = allocate_reg();
        code_buffer.emit(new_reg + " = and i32 " + result->reg + ", 255");
        result->reg = new_reg;
    }
}


void codeGen_chan::gen_relop(Node *result, Node *left, string op_str, Node *right){
    string llvm_op;
    if (op_str == "!=") {
        llvm_op = "ne";
    } else if (op_str == "==") {
        llvm_op = "eq";
    } else if (op_str == "<") {
        llvm_op = "slt";
    } else if(op_str == "<="){
        llvm_op = "sle";
    } else if (op_str == ">") {
        llvm_op = "sgt";
    } else {
        llvm_op = "sge";
    }

    result->reg = allocate_reg();
    code_buffer.emit(result->reg + " = icmp " + llvm_op + " i32 " + left->reg + ", " + right->reg + '\n');
}


void codeGen_chan::load_from_stack(Node* id_node, int offset){
    string ptr_reg = allocate_reg();
    id_node->reg = allocate_reg();
    code_buffer.emit(ptr_reg + " = getelementptr i32, i32* %frame_chan, i32 " + std::to_string(offset));
    if (id_node->type == "BOOL")
    {
        string trunc_reg = allocate_reg();
        code_buffer.emit(trunc_reg + " = load i32, i32* " + ptr_reg);
        code_buffer.emit(id_node->reg + " = trunc i32 " + trunc_reg + " to i1");
    } else {
        code_buffer.emit(id_node->reg + " = load i32, i32* " + ptr_reg);
    }
}

void codeGen_chan::assign_reg_for_literal(Node* num_node, string val){
    num_node->reg = allocate_reg();
    code_buffer.emit(num_node->reg + " = add i32 0, " + val);
}

void codeGen_chan::assign_reg_for_bool_literal(Node* num_node, string val){
    num_node->reg = allocate_reg();
    code_buffer.emit(num_node->reg + " = add i1 0, " + val);
}

void codeGen_chan::store_to_stack(int offset, Node* assigned_node)
{
    string ptr_reg = allocate_reg();
    code_buffer.emit(ptr_reg + " = getelementptr i32, i32* %frame_chan, i32 " + std::to_string(offset));
    if(assigned_node->type=="BOOL")
    {
        string zext_reg = allocate_reg();
        code_buffer.emit(zext_reg + " = zext i1 " + assigned_node->reg + " to i32");
        code_buffer.emit("store i32 " + zext_reg + ", i32* " + ptr_reg);
    } else {
        code_buffer.emit("store i32 " + assigned_node->reg + ", i32* " + ptr_reg);
    }
}

void codeGen_chan::gen_call_function(Node *function, Node *argument, Node *ret_node)
{
    ret_node->reg = allocate_reg();
    if(function->value == "readi"){
        code_buffer.emit(ret_node->reg + " = call i32 @" + function->value + "(i32" + argument->reg + ")");
    } else if (function->value == "printi") {
        code_buffer.emit("call void @" + function->value + "(i32 " + argument->reg + ")");
    } else {
        code_buffer.emit("call void @" + function->value + "(i8* " + argument->reg + ")");
    }
}

void codeGen_chan::add_while_labels(Node *bool_exp)
{
    while_labels_list_chan.back().second = bool_exp->false_label;
    code_buffer.emit(bool_exp->tmp_code_buffer);
    code_buffer.emit(bool_exp->true_label + ":");
}

void codeGen_chan::pre_while()
{
    string label = code_buffer.freshLabel();
    code_buffer.emit("br label %" + label);
    code_buffer.emit(label + ":");
    while_labels_list_chan.push_back(std::pair<string, string>(label, ""));
}

void codeGen_chan::add_break(){
    code_buffer.emit("br label %" + while_labels_list_chan.back().second);
}

void codeGen_chan::add_continue(){
    code_buffer.emit("br label %" + while_labels_list_chan.back().first);
}

void codeGen_chan::end_while_actions()
{
    code_buffer.emit("br label %" + while_labels_list_chan.back().first);
    code_buffer.emit(while_labels_list_chan.back().second + ":");
    while_labels_list_chan.pop_back();
}

void codeGen_chan::add_if_labels(Node* bool_exp)
{
    code_buffer.emit(bool_exp->tmp_code_buffer);
    if_labels_list_chan.push_back(std::pair<string, string>(bool_exp->false_label, code_buffer.freshLabel()));
    code_buffer.emit(bool_exp->true_label + ":");
}

void codeGen_chan::end_if_actions()
{
    code_buffer.emit("br label %" + if_labels_list_chan.back().first);
    code_buffer.emit(if_labels_list_chan.back().first + ":");
    if_labels_list_chan.pop_back();
}

void codeGen_chan::skip_else()
{
    code_buffer.emit("br label %" + if_labels_list_chan.back().second);
    code_buffer.emit(if_labels_list_chan.back().first + ":");
}

void codeGen_chan::end_else_actions()
{
    code_buffer.emit("br label %" + if_labels_list_chan.back().second);
    code_buffer.emit(if_labels_list_chan.back().second + ":");
    if_labels_list_chan.pop_back();
}

void codeGen_chan::add_return()
{
    code_buffer.emit("ret i32 0");
}

void codeGen_chan::add_closing_scope()
{
    code_buffer.emit("ret i32 0");
    code_buffer.emit("}");
}

void codeGen_chan::add_global_string(Node *node)
{
    node->value.erase(0, 1);
    node->value.pop_back();
    int str_size = node->value.length() + 1;
    code_buffer.emitGlobal("@.str" + std::to_string(str_index++) + " = constant [" + std::to_string(str_size)+ " x i8] c\"" + node->value + "\\00\"");
    node->reg = allocate_reg();
    code_buffer.emit(node->reg + " = getelementptr [" + std::to_string(str_size) + " x i8], [" + std::to_string(str_size) + " x i8]* @.str" + std::to_string(str_index - 1) + ", i32 0, i32 0");
}

void codeGen_chan::store_bool_stack(int offset, int bool_value)
{
    string ptr_reg = allocate_reg();
    code_buffer.emit(ptr_reg + " = getelementptr i32, i32* %frame_chan, i32 " + std::to_string(offset));
    code_buffer.emit("store i32 " + std::to_string(bool_value) + ", i32* " + ptr_reg);
}
