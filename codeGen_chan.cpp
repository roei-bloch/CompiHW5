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
    code_buffer.emitGlobal("    %cond = icmp eq %0, 0");
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


void codeGen_chan::gen_binop(EXP *result, EXP *left, string op, EXP *right){
    
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