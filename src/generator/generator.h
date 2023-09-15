#pragma once

#include "scanning/token.h"

#include <stack>
#include <fstream>

namespace Generator {
  using Reg = int; // TODO: Maybe replace with a tuple that includes both the reg # and type info

  static std::stack<Reg> register_stack;
  static std::ofstream *llvmir_file;

  static void generate_llvm_initialize() {
    llvmir_file = new std::ofstream("program_ir.ll");

    *llvmir_file << "; ModuleID = 'examples/test1'\n"
                 << ";source_filename = \"examples/test1\"\n" 
                 << "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n"
                 << "target triple = \"x86_64-pc-linux-gnu\"\n"
                 << "@print_int_fstring = private unnamed_addr constant [4 x i8] c\"%d\\0A\\00\", align 1\n"
                 << "; Function Attrs: noinline nounwind optnone uwtable\n"
                 << "define dso_local i32 @main() #0 {\n";
  }

  static void generate_llvm(Token::Token token) {
    using namespace Token;

    static Reg next_reg = 1;

    std::visit(overloaded {
      [&](Plus&) {
        *llvmir_file << "%" << next_reg << " = add nsw i32 %" << register_stack.top();
        register_stack.pop();
        *llvmir_file << ", %" << register_stack.top() << "\n";
        register_stack.pop();
        register_stack.push(next_reg);
        next_reg++;
      },
      [&](Minus&) {
        *llvmir_file << "%" << next_reg << " = sub nsw i32 %" << register_stack.top();
        register_stack.pop();
        *llvmir_file << ", %" << register_stack.top() << "\n";
        register_stack.pop();
        register_stack.push(next_reg);
        next_reg++;
      },
      [&](Star&) {
        *llvmir_file << "%" << next_reg << " = mul nsw i32 %" << register_stack.top();
        register_stack.pop();
        *llvmir_file << ", %" << register_stack.top() << "\n";
        register_stack.pop();
        register_stack.push(next_reg);
        next_reg++;
      },
      [&](Slash&) {
        *llvmir_file << "%" << next_reg << " = udiv i32 %" << register_stack.top();
        register_stack.pop();
        *llvmir_file << ", %" << register_stack.top() << "\n";
        register_stack.pop();
        register_stack.push(next_reg);
        next_reg++;
      },
      [&](IntegerLiteral &t) {
        *llvmir_file << "%" << next_reg << " = alloca i32, align 4\n"
                     << "store i32 " << t.value << ", i32* %" << next_reg << ", align 4\n"
                     << "%" << next_reg + 1 << " = load i32, i32* %" << next_reg << ", align 4\n";
        register_stack.push(next_reg + 1);
        next_reg += 2;
      },
      [&](auto&) { throw std::runtime_error("LLVM generation unimplemented for this token"); }
    }, token);
  }

  static void generate_llvm_finalize() {
    *llvmir_file << "call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @print_int_fstring , i32 0, i32 0), i32 %" << register_stack.top() << ")\n";
    register_stack.pop();
    *llvmir_file << "ret i32 0\n"
                 << "}\n"
                 << "declare i32 @printf(i8*, ...) #1\n"
                 << std::endl;

    llvmir_file->close();
  }
}
