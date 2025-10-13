#include "semantic_analyzer.h"
#include "fmt/format.h"

void SemanticAnalyzer::analyze_program(Program &ast) {
  scope_stack.push_back(
      ScopeInfo(ScopeType::Global)); // Push global scope onto scope stack.

  for (const auto &node : ast.statements) {
    analyze_symbols(node);
  }
}

void SemanticAnalyzer::analyze_symbols(
    std::unique_ptr<StatementNode> const &node) {
  if (auto var_decl = dynamic_cast<VariableDeclarationNode *>(node.get())) {
    std::string var_name = var_decl->variable->name;
    fmt::println("Found variable declaration: {}", var_name);
    Datatype var_type = var_decl->datatype;
    if (scope_stack.back().variables.contains(var_name) ||
        scope_stack.back().functions.contains(var_name)) {
      throw std::runtime_error(
          "Duplicate symbol declaration found for variable {}." + var_name);
    }
    fmt::println("No previous declaration of variable {} found.", var_name);

    auto var_info = std::make_shared<VariableInfo>(VariableInfo{});
    var_info->name = var_name;
    // Check which scope kind we are in
    if (scope_stack.back().type == ScopeType::Global) {
      var_info->is_global = true;
    } else {
      if (var_type == Datatype::INTEGER) {
        scope_stack.back().stack_size += 4; // Integer size is 4 bytes.
      } else {
        throw std::runtime_error(
            "Unable to increment stack size as type is not implemented");
      }
      var_info->stack_offset = scope_stack.back().stack_size;
    }
    scope_stack.back().variables[var_name] = var_info;
    var_decl->variable->variable_annotation =
        var_info; // Annotate identifier node.
  } else {
    fmt::println("Statement is not a declaration");
  }
}
