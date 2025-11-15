#pragma once

#include "node.h"
#include "symbol.h"
#include <memory>
#include <vector>

class SemanticAnalyzer {
public:
  void analyze_program(ScopeNode *ast);

  std::vector<ScopeInfo> scope_stack;
  std::map<std::string, FunctionInfo> functions;

  void analyze_scope(ScopeNode *ast);

  void analyze_var_decl(VariableDeclarationNode *var_decl);
  void analyze_func_decl(FunctionDeclarationNode *func_decl);
  void analyze_return_stmt(ReturnNode *return_stmt);

  std::shared_ptr<VariableInfo> get_var_info(IdentifierNode *identifier);
};