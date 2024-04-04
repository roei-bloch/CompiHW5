declare i32 @scanf(i8*, ...)
declare i32 @printf(i8*, ...)
declare void @exit(i32)
@.int_specifier_scan = constant [3 x i8] c"%d\00"
@.int_specifier = constant [4 x i8] c"%d\0A\00"
@.str_specifier = constant [4 x i8] c"%s\0A\00"
@.invalid_div_msg = constant [23 x i8] c"Error division by zero\00"

define i32 @readi(i32) {
    %ret_val = alloca i32
    %spec_ptr = getelementptr [3 x i8], [3 x i8]* @.int_specifier_scan, i32 0, i32 0
    call i32 (i8*, ...) @scanf(i8* %spec_ptr, i32* %ret_val)
    %val = load i32, i32* %ret_val
    ret i32 %val
}

define void @printi(i32) {
    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0
    call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)
    ret void
}

define void @print(i8*) {
    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0
    call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)
    ret void
}
define void @validatedevision(i32) {
    %cond = icmp eq i32 %0, 0
    br i1 %cond, label %Invalid_div, label %Valid_div
Invalid_div:
    %msg_ptr = getelementptr [23 x i8], [23 x i8]* @.invalid_div_msg, i32 0, i32 0
    call void (i8*) @print(i8* %msg_ptr)
    call void (i32) @exit(i32 0)
    ret void
Valid_div:
    ret void
}
@.str0 = constant [12 x i8] c"METZAHKEKET\00"
define i32 @main() {
%arr_size = add i32 50, 0
%frame_chan = alloca i32, i32 %arr_size
%reg0 = add i1 0, 0
%reg1 = getelementptr i32, i32* %frame_chan, i32 0
%reg2 = zext i1 %reg0 to i32
store i32 %reg2, i32* %reg1
%reg3 = add i1 0, 1
br i1 %reg3, label %label_4, label %label_5

label_4:
%reg4 = add i1 0, 1
%reg5 = getelementptr i32, i32* %frame_chan, i32 0
%reg6 = zext i1 %reg4 to i32
store i32 %reg6, i32* %reg5
br label %label_6
label_5:
%reg7 = getelementptr i32, i32* %frame_chan, i32 0
%reg9 = load i32, i32* %reg7
%reg8 = trunc i32 %reg9 to i1
br i1 %reg4, label %label_7, label %label_8

label_7:
%reg10 = getelementptr [12 x i8], [12 x i8]* @.str0, i32 0, i32 0
call void @print(i8* %reg10)
br label %label_8
label_8:
br label %label_6
label_6:
%reg12 = getelementptr i32, i32* %frame_chan, i32 0
store i32 0, i32* %reg12
ret i32 0
}
