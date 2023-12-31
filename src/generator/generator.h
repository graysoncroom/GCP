#pragma once

#include "scanning/token.h"

#include <stack>
#include <fstream>
#include <stdexcept>

// TODO: Horrible way of abstracting a Generator. Fix! Singleton class? Idk.
// What's the problem? register_stack and llvmir_file pointer are accessible outside this file
namespace Generator {
  using Reg = int; // TODO: Maybe replace with a tuple that includes both the reg # and type info

  // TODO: I don't even really know what static means inside a namespace...
  static std::stack<Reg> register_stack;
  static std::ofstream *llvmir_file;

  static void generate_llvm_initialize() {
    llvmir_file = new std::ofstream("a.ll");

    *llvmir_file << "; ModuleID = 'examples/test1'\n"
                 << ";source_filename = \"examples/test1\"\n" 
                 << "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n"
                 << "target triple = \"x86_64-pc-linux-gnu\"\n"
                 << "@print_int_fstring = private unnamed_addr constant [4 x i8] c\"%d\\0A\\00\", align 1\n"
                 << "; Function Attrs: noinline nounwind optnone uwtable\n"
                 << "define dso_local i32 @main() #0 {\n";
  }

  static void generate_llvm(const Token::Token& token) {
    using namespace Token;

    static Reg next_reg = 1;

    std::visit(overloaded {
      [&](const Plus&) {
        auto rhs = register_stack.top();
        register_stack.pop();
        auto lhs = register_stack.top();
        register_stack.pop();

        *llvmir_file << "%" << next_reg << " = add nsw i32 %" << lhs << ", %" << rhs << "\n";
        register_stack.push(next_reg);
        next_reg++;
      },
      [&](const Minus&) {
        auto rhs = register_stack.top();
        register_stack.pop();
        auto lhs = register_stack.top();
        register_stack.pop();

        *llvmir_file << "%" << next_reg << " = sub nsw i32 %" << lhs << ", %" << rhs << "\n";
        register_stack.push(next_reg);
        next_reg++;
      },
      [&](const Star&) {
        auto rhs = register_stack.top();
        register_stack.pop();
        auto lhs = register_stack.top();
        register_stack.pop();

        *llvmir_file << "%" << next_reg << " = mul nsw i32 %" << lhs << ", %" << rhs << "\n";
        register_stack.push(next_reg);
        next_reg++;
      },
      [&](const Slash&) {
        auto rhs = register_stack.top();
        register_stack.pop();
        auto lhs = register_stack.top();
        register_stack.pop();

        *llvmir_file << "%" << next_reg << " = udiv i32 %" << lhs << ", %" << rhs << "\n";
        register_stack.push(next_reg);
        next_reg++;
      },
      [&](const Print&) {
        *llvmir_file << "call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @print_int_fstring , i32 0, i32 0), i32 %" << register_stack.top() << ")\n";
        register_stack.pop();
        next_reg++;
      },
      [&](const IntegerLiteral &t) {
        *llvmir_file << "%" << next_reg << " = alloca i32, align 4\n"
                     << "store i32 " << t.value << ", i32* %" << next_reg << ", align 4\n"
                     << "%" << next_reg + 1 << " = load i32, i32* %" << next_reg << ", align 4\n";
        register_stack.push(next_reg + 1);
        next_reg += 2; //since each loading requires 1 reg for ptr and 1 reg for val
      },
      [&](const UnknownToken&) {},
      [&](const auto&) { throw std::runtime_error("[Generator::generate_llvm] Undefined LLVM-IR translation for token"); }
    }, token);
  }

  static void generate_llvm_finalize() {
    *llvmir_file << "ret i32 0\n"
                 << "}\n"
                 << "declare i32 @printf(i8*, ...) #1\n"
                 << std::endl;

    llvmir_file->close();
  }
}
