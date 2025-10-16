#include "semantic_analyzer.h"
#include "fmt/format.h"

void SemanticAnalyzer::analyze_scope(ScopeNode *ast) {
  for (const auto &node : ast->statements) {
    if (auto var_decl = dynamic_cast<VariableDeclarationNode *>(node.get())) {
      analyze_var_decl(var_decl);
    } else if (auto scope = dynamic_cast<ScopeNode *>(node.get())) {
      scope_stack.push_back(ScopeInfo(ScopeType::Block)); // Enter block scope
      analyze_scope(scope);
    } else {
      throw std::runtime_error(
          "Encountered uninmplemented statement type during analysis");
    }
  }
}

void SemanticAnalyzer::analyze_program(ScopeNode *ast) {
  scope_stack.push_back(
      ScopeInfo(ScopeType::Global)); // Push global scope onto scope stack.

  analyze_scope(ast);
}

void SemanticAnalyzer::analyze_var_decl(VariableDeclarationNode *var_decl) {
  std::string var_name = var_decl->variable->name;
  Datatype var_type = var_decl->datatype;
  if (var_type != Datatype::INTEGER) {
    throw std::runtime_error(
        fmt::format("Unimplemented variable type for var {}", var_name));
  }

  if (scope_stack.back().variables.contains(var_name)) {
    throw std::runtime_error(
        "Duplicate symbol declaration found for variable {}." + var_name);
  }

  auto var_info = std::make_shared<VariableInfo>(VariableInfo{});
  var_info->name = var_name;
  // Check if in global scope
  if (scope_stack.back().type == ScopeType::Global) {
    var_info->is_global = true;
  } else {
    var_info->stack_offset = scope_stack.back().stack_size;
    scope_stack.back().stack_size += 4; // Integer size is 4 bytes.
  }
  scope_stack.back().variables[var_name] = var_info;
  var_decl->variable->variable_annotation = var_info; // Annotate node
}
