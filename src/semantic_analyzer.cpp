#include "semantic_analyzer.h"
#include "fmt/format.h"

void SemanticAnalyzer::analyze_scope(ScopeNode *ast) {
  for (const auto &node : ast->statements) {
    if (auto var_decl = dynamic_cast<VariableDeclarationNode *>(node.get())) {
      analyze_var_decl(var_decl);
    } else if (auto func_decl =
                   dynamic_cast<FunctionDeclarationNode *>(node.get())) {
      analyze_func_decl(func_decl);
    } else if (auto scope = dynamic_cast<ScopeNode *>(node.get())) {
      scope_stack.push_back(ScopeInfo(ScopeType::Block)); // Enter block scope
      analyze_scope(scope);
    } else {
      throw std::runtime_error(
          "Encountered uninmplemented statement type during analysis");
    }
  }
  ast->scope_annotation = scope_stack.back(); // Annotate scope
  scope_stack.pop_back();                     // Exit scope
}

void SemanticAnalyzer::analyze_program(ScopeNode *ast) {
  scope_stack.push_back(ScopeInfo(ScopeType::Global)); // Enter global scope

  analyze_scope(ast);

  if (!functions.contains("huvud")) {
    throw std::runtime_error("No main function found");
  } else {
    if (functions["huvud"].return_type != Datatype::INTEGER) {
      throw std::runtime_error("Main function must return integer");
    }
    if (!functions["huvud"].param_types.empty()) {
      throw std::runtime_error("Main function can not have parameters");
    }
  }
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
        "Duplicate variable declaration found for variable {}." + var_name);
  }

  auto var_info = std::make_shared<VariableInfo>(VariableInfo{});
  var_info->name = var_name;
  // Check if in global scope
  if (scope_stack.back().type == ScopeType::Global) {
    var_info->is_global = true;
    fmt::println("{} in global scope", var_name);
  } else {
    fmt::println("{} in local/function scope", var_name);
    var_info->stack_offset = scope_stack.back().stack_size;
    scope_stack.back().stack_size += 4; // Integer size is 4 bytes.
    fmt::println("Stack size now {}", scope_stack.back().stack_size);
  }
  scope_stack.back().variables[var_name] = var_info;
  var_decl->variable->variable_annotation = var_info; // Annotate node
}

void SemanticAnalyzer::analyze_func_decl(FunctionDeclarationNode *func_decl) {
  std::string func_name = func_decl->functionName->name;
  Datatype return_type = func_decl->returnType;
  auto func_def = dynamic_cast<FunctionDefinitionNode *>(func_decl);
  if (functions.contains(func_name)) {
    // If this a definition, a previous declaration is OK
    if (func_def) {
      if (functions[func_name].defined) {
        throw std::runtime_error(
            fmt::format("Duplicate function definition: {}", func_name));
      }
      auto &def_params = func_def->parameters;
      auto decl_types = functions[func_name].param_types;

      if (def_params.size() != decl_types.size()) {
        throw std::runtime_error(
            fmt::format("Mismatch for number of params: {}", func_name));
      }
      for (size_t i = 0; i < decl_types.size(); i++) {
        if (decl_types[i] != def_params[i]->datatype) {
          throw std::runtime_error(fmt::format(
              "Parameter type mismatch for function {}", func_name));
        }
      }
      // If this is a declaration, a previous declaration is NOK
    } else {
      throw std::runtime_error(
          fmt::format("Duplicate function declaration: {}", func_name));
    }
    // no previous func def/decl exists
  } else {
    std::vector<Datatype> param_types;
    for (auto &param : func_decl->parameters) {
      auto data_type = param->datatype;
      if (data_type != Datatype::INTEGER) {
        throw std::runtime_error(
            fmt::format("Unsupported data type in func {}", func_name));
      }
      param_types.push_back(data_type);
    }
    functions[func_name] = FunctionInfo{.name = func_name,
                                        .return_type = return_type,
                                        .param_types = param_types,
                                        .defined = func_def != nullptr};
  }
  if (func_def) {
    fmt::println("Going into function scope");
    scope_stack.push_back(
        ScopeInfo{ScopeType::Function}); // Enter function scope
    for (const auto &param : func_def->parameters) {
      auto var_info =
          VariableInfo{.name = param->name->name,
                       .type = param->datatype,
                       .stack_offset = scope_stack.back().stack_size};
      auto var_info_ptr = std::make_shared<VariableInfo>(var_info);
    }
    analyze_scope(func_def->body.get());
  }
}
