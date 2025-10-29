#include "codegen.h"
#include "node.h"

std::string Codegen::generate_assembly(const ScopeNode &program) {

  text.clear();
  data.clear();
  bss.clear();

  text << ".text\n";
  text << ".globl _start\n";
  text << ".globl huvud\n\n";

  text << "_start:\n";
  text << "    call huvud\n";
  text << "    mov %rax, %rdi\n"; // move main function return value to exit
                                  // status";
  text << "    mov $60, %rax\n";  // syscall number for exit"
  text << "    syscall\n";        // invoke kernel;

  generate_scope(program);

  return text.str();
}

void Codegen::generate_statement(const StatementNode &node,
                                 const std::string &exit_label) {
  if (auto *func_def = dynamic_cast<const FunctionDefinitionNode *>(&node)) {
    generate_function_def(*func_def, exit_label);
  } else if (auto *ret_node = dynamic_cast<const ReturnNode *>(&node)) {
    generate_return_node(*ret_node, exit_label);
  } else {
    throw std::runtime_error(fmt::format(
        "Node {} not implemented in code generation", to_string(node)));
  }
}

void Codegen::generate_function_def(const FunctionDefinitionNode &node,
                                    const std::string &exit_label) {
  text << node.functionName->name << ":"
       << "\n";

  text << "    push %rbp\n";      // save caller base pointer
  text << "    mov %rsp, %rbp\n"; // set base pointer to top of stack

  const auto local_exit_label =
      fmt::format(".exit_{}", node.functionName->name);

  if (!node.body) {
    throw std::runtime_error(
        fmt::format("Function {} has null body", node.functionName->name));
  }
  generate_scope(*node.body, local_exit_label);

  text << local_exit_label << ":\n";
  text << "    mov %rbp, %rsp\n"; // restore stack pointer
  text << "    pop %rbp\n";       // restore caller base pointer
  text << "    ret\n";
}

void Codegen::generate_scope(const ScopeNode &node,
                             const std::string &exit_label) {
  for (const auto &stmt : node.statements) {
    generate_statement(*stmt, exit_label);
  }
}

void Codegen::generate_return_node(const ReturnNode &node,
                                   const std::string &exit_label) {
  if (const auto *int_lit =
          dynamic_cast<const IntegerLiteralNode *>(node.expression.get())) {
    text << "    movq $" << int_lit->value << ", %rax\n";
  } else {
    throw std::runtime_error(
        "Only integer literals are supported in return nodes so far");
  }
  text << "    jmp " << exit_label << "\n";
}
