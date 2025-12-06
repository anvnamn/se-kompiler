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

  data << ".data\n";

  bss << ".bss\n";

  generate_scope(program);

  return text.str() + data.str() + bss.str();
}

void Codegen::generate_statement(const StatementNode &node) {
  if (auto *func_def = dynamic_cast<const FunctionDefinitionNode *>(&node)) {
    generate_function_def(*func_def);
  } else if (auto *ret_node = dynamic_cast<const ReturnNode *>(&node)) {
    generate_return_node(*ret_node);
  } else if (auto *variable_decl =
                 dynamic_cast<const VariableDeclarationNode *>(&node)) {
    generate_variable_declaration(*variable_decl);
  } else {
    throw std::runtime_error(fmt::format(
        "Node {} not implemented in code generation", to_string(node)));
  }
}

void Codegen::generate_function_def(const FunctionDefinitionNode &node) {
  text << node.functionName->name << ":"
       << "\n";

  text << "    push %rbp\n";      // save caller base pointer
  text << "    mov %rsp, %rbp\n"; // set base pointer to top of stack

  if (!node.body) {
    throw std::runtime_error(
        fmt::format("Function {} has null body", node.functionName->name));
  }

  if (!node.body->scope_annotation) {
    throw std::runtime_error(fmt::format("Function {} has no scope annotation",
                                         node.functionName->name));
  }

  // Allocate stack space for local variables
  int local_var_size = node.body->scope_annotation->stack_size;
  // Make sure local_var_size is a multiple of 16
  constexpr int stack_alignment = 16;
  int remainder = local_var_size % stack_alignment;
  if (remainder != 0) {
    local_var_size += (stack_alignment - remainder);
  }
  if (local_var_size > 0) {
    text << fmt::format("    sub ${}, %rsp\n", local_var_size);
  }

  generate_scope(*node.body);

  if (!node.return_label) {
    throw std::runtime_error(fmt::format("Function {} has no return label",
                                         node.functionName->name));
  }

  text << *node.return_label << ":\n";
  text << "    mov %rbp, %rsp\n"; // restore stack pointer
  text << "    pop %rbp\n";       // restore caller base pointer
  text << "    ret\n";
}

void Codegen::generate_scope(const ScopeNode &node) {
  for (const auto &stmt : node.statements) {
    generate_statement(*stmt);
  }
}

void Codegen::generate_return_node(const ReturnNode &node) {
  if (const auto *int_lit =
          dynamic_cast<const IntegerLiteralNode *>(node.expression.get())) {
    text << "    movq $" << int_lit->value << ", %rax\n";
  } else if (const auto *identifier =
                 dynamic_cast<const IdentifierNode *>(node.expression.get())) {
    const auto var_info = identifier->variable_annotation;
    if (!var_info) {
      throw std::runtime_error(
          fmt::format("Variable {} in return statement is not annotated",
                      identifier->name));
    }
    if (var_info->is_global) {
      text << fmt::format("    movl {}(%rip), %eax\n", var_info->name);
    } else {
      if (var_info->type == Datatype::INTEGER) {
        text << fmt::format("    movl -{}(%rbp), %eax\n",
                            var_info->stack_offset);
      } else {
        throw std::runtime_error("Unsupported local variable type in return");
      }
    }
  } else {
    throw std::runtime_error("Unsupported return expression type");
  }

  if (!node.return_label) {
    throw std::runtime_error("Return node has no return label");
  }
  text << "    jmp " << *node.return_label << "\n";
}

void Codegen::generate_variable_declaration(
    const VariableDeclarationNode &node) {
  const auto var_info = node.variable->variable_annotation;
  if (!var_info) {
    throw std::runtime_error(
        fmt::format("Variable {} has no annotation", node.variable->name));
  }

  if (var_info->is_global) {
    if (const auto var_init =
            dynamic_cast<const VariableInitializationNode *>(&node)) {
      // Global variable definition with initialization
      if (const auto int_lit =
              dynamic_cast<const IntegerLiteralNode *>(var_init->expr.get())) {
        data << var_info->name << ":\n";
        data << fmt::format("    .long {}\n", int_lit->value);
        return;
      }
      throw std::runtime_error(
          "Only integer literals are supported in global variable "
          "initializations so far");
    } else {
      // Global variable declaration without initialization
      bss << fmt::format("    .lcomm {}, 4\n",
                         var_info->name); // 4 bytes for integer
    }
  } else {
    if (const auto var_init =
            dynamic_cast<const VariableInitializationNode *>(&node)) {
      if (const auto int_lit =
              dynamic_cast<IntegerLiteralNode *>(var_init->expr.get())) {
        text << fmt::format("    movl ${}, -{}(%rbp)\n", int_lit->value,
                            var_info->stack_offset);
      } else {
        throw std::runtime_error(
            "Only integer literals are supported in local variable "
            "initializations so far");
      }
    } else {
      return; // No code needed for local variable declaration without
              // initialization
    }
  }
}