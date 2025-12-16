#include "codegen.h"
#include "node.h"

std::string Codegen::generate_assembly(ScopeNode &program) {

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

  program.accept(this);
  return text.str() + data.str() + bss.str();
}

void Codegen::visit_scope_node(ScopeNode *node) {
  for (const auto &stmt : node->statements) {
    stmt->accept(this);
  }
}

void Codegen::visit_func_def_node(FunctionDefinitionNode *node) {
  text << node->functionName->name << ":"
       << "\n";

  text << "    push %rbp\n";      // save caller base pointer
  text << "    mov %rsp, %rbp\n"; // set base pointer to top of stack

  if (!node->body) {
    throw std::runtime_error(
        fmt::format("Function {} has null body", node->functionName->name));
  }

  if (!node->body->scope_annotation) {
    throw std::runtime_error(fmt::format("Function {} has no scope annotation",
                                         node->functionName->name));
  }

  // Allocate stack space for local variables
  int local_var_size = node->body->scope_annotation->stack_size;
  // Make sure local_var_size is a multiple of 16
  constexpr int stack_alignment = 16;
  int remainder = local_var_size % stack_alignment;
  if (remainder != 0) {
    local_var_size += (stack_alignment - remainder);
  }
  if (local_var_size > 0) {
    text << fmt::format("    sub ${}, %rsp\n", local_var_size);
  }

  node->body->accept(this); // Visit function body

  if (node->body->scope_annotation->return_label.empty()) {
    throw std::runtime_error(fmt::format(
        "Function {}'s body has no return label", node->functionName->name));
  }

  text << node->body->scope_annotation->return_label << ":\n";
  text << "    mov %rbp, %rsp\n"; // restore stack pointer
  text << "    pop %rbp\n";       // restore caller base pointer
  text << "    ret\n";
}

void Codegen::visit_return_node(ReturnNode *node) {
  if (const auto *int_lit =
          dynamic_cast<const IntegerLiteralNode *>(node->expression.get())) {
    text << "    movq $" << int_lit->value << ", %rax\n";
  } else if (const auto *identifier =
                 dynamic_cast<const IdentifierNode *>(node->expression.get())) {
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
        text << fmt::format("    movl {}(%rbp), %eax\n",
                            var_info->stack_offset);
      } else {
        throw std::runtime_error("Unsupported local variable type in return");
      }
    }
  } else {
    throw std::runtime_error("Unsupported return expression type");
  }

  if (!node->return_label) {
    throw std::runtime_error("Return node has no return label");
  }
  text << "    jmp " << *node->return_label << "\n";
}

void Codegen::visit_var_decl_node(VariableDeclarationNode *node) {

  const auto var_info = node->variable->variable_annotation;
  if (!var_info) {
    throw std::runtime_error(
        fmt::format("Variable {} has no annotation", node->variable->name));
  }
  // Local vars need no code generated for declarations
  if (var_info->is_global) {
    if (var_info->type == Datatype::INTEGER) {
      bss << fmt::format("    .lcomm {}, 4\n",
                         var_info->name); // 4 bytes for integer
    } else {
      throw std::runtime_error(
          "Only integer literals are supported in variable "
          "initializations so far");
    }
  }
}

void Codegen::visit_int_literal_node(IntegerLiteralNode *node) {
  // Integer literals are typically handled in context (e.g., in return
  // statements) This is a no-op in most cases
}

void Codegen::visit_identifier_node(IdentifierNode *node) {
  // Identifiers are typically handled in context
  // This is a no-op in most cases
}

void Codegen::visit_parameter_node(ParameterNode *node) {
  // Parameters are handled during function definition setup
  // This is a no-op in most cases
}

void Codegen::visit_func_decl_node(FunctionDeclarationNode *node) {
  // Function declarations without definitions don't generate code
}

void Codegen::visit_assignment_node(AssignmentNode *node) {
  // Assignment handling can be added here when needed
  throw std::runtime_error("Assignment code generation not yet implemented");
}

void Codegen::visit_var_init_node(VariableInitializationNode *node) {
  const auto var_info = node->variable->variable_annotation;
  if (!var_info) {
    throw std::runtime_error(
        fmt::format("Variable {} has no annotation", node->variable->name));
  }

  if (const auto int_lit =
          dynamic_cast<const IntegerLiteralNode *>(node->expr.get())) {
    if (var_info->is_global) {
      data << var_info->name << ":\n";
      data << fmt::format("    .long {}\n", int_lit->value);
      return;
    } else {
      text << fmt::format("    movl ${}, {}(%rbp)\n", int_lit->value,
                          var_info->stack_offset);
      return;
    }
  } else {
    throw std::runtime_error("Only integer literals are supported in variable "
                             "initializations so far");
  }
}