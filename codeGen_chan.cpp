#include "codeGen_chan.h"
#include "bp.hpp"

codeGen_chan::codeGen_chan()
{
    code_buffer = CodeBuffer();
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
    code_buffer.emitGlobal("    %cond = icmp i32 eq %0, 0");
    code_buffer.emitGlobal("    br i1 %cond, label %Invalid_div, label %Valid_div");
    code_buffer.emitGlobal("Invalid_div:");
    code_buffer.emitGlobal("    %msg_ptr = getelementptr [23 x i8], [23 x i8]* @.invalid_div_msg, i32 0, i32 0");
    code_buffer.emitGlobal("    call i32 (i8*, ...) @print(i8* %msg_ptr)");
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
    code_buffer.emit("%frame_chan = alloca i32, i32* %arr_size");
}

void codeGen_chan::clean_offset(int offset)
{
    string reg = allocate_reg();
    code_buffer.emit(reg + " = getelementptr i32, i32* %frame_chan, i32 " + std::to_string(offset));
    code_buffer.emit("store i32 0, i32* " + reg);
}



void codeGen_chan::gen_binop(EXP *result, EXP *left, string op_str, EXP *right){
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
        code_buffer.emit(new_reg + " = and i32 255, " + result->reg);
        result->reg = new_reg;
    }
}


void codeGen_chan::gen_relop(EXP *result, EXP *left, string op_str, EXP *right){
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
    code_buffer.emit(result->reg + " = icmp " + llvm_op + " i32 " + left->reg + ", " + right->reg);
}


void codeGen_chan::load_from_stack(Node* id_node, int offset){
    string ptr_reg = allocate_reg();
    id_node->reg = allocate_reg();
    code_buffer.emit(ptr_reg + " = getelementptr i32, i32* %frame_chan, i32 " + std::to_string(offset));
    code_buffer.emit(id_node->reg + " = load i32, i32* " + ptr_reg);
}

void codeGen_chan::assign_reg_for_literal(Node* num_node, string val){
    num_node->reg = allocate_reg();
    code_buffer.emit(num_node->reg + "add i32 0, " + val);
}

void codeGen_chan::store_to_stack(int offset, Node* assigned_node)
{
    string ptr_reg = allocate_reg();
    code_buffer.emit(ptr_reg + " = getelementptr i32, i32* %frame_chan, i32 " + std::to_string(offset));
    code_buffer.emit("store i32 " + assigned_node->reg + "i32* " + ptr_reg);
}