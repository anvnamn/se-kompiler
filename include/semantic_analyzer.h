#pragma once

#include "node.h"
#include "symbol.h"
#include <memory>
#include <vector>

class SemanticAnalyzer : public Visitor {
public:
  void analyze_program(ScopeNode *ast);

  std::vector<ScopeInfo> scope_stack;
  std::map<std::string, FunctionInfo> functions;

  void visit_scope_node(ScopeNode *ast) override;

  void visit_var_decl_node(VariableDeclarationNode *node) override;
  void visit_var_init_node(VariableInitializationNode *node) override;
  void visit_func_decl_node(FunctionDeclarationNode *node) override;
  void visit_func_def_node(FunctionDefinitionNode *node) override;
  void visit_return_node(ReturnNode *node) override;

  std::shared_ptr<VariableInfo> get_var_info(IdentifierNode *identifier);
};