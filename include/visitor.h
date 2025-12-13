#pragma once

class IntegerLiteralNode;
class ScopeNode;
class IdentifierNode;
class ParameterNode;
class FunctionDeclarationNode;
class FunctionDefinitionNode;
class ReturnNode;
class AssignmentNode;
class VariableDeclarationNode;
class VariableInitializationNode;

class Visitor {
public:
  virtual void visit_int_literal_node(const IntegerLiteralNode *node) = 0;
  virtual void visit_scope_node(const ScopeNode *node) = 0;
  virtual void visit_identifier_node(const IdentifierNode *node) = 0;
  virtual void visit_parameter_node(const ParameterNode *node) = 0;
  virtual void visit_func_decl_node(const FunctionDeclarationNode *node) = 0;
  virtual void visit_func_def_node(const FunctionDefinitionNode *node) = 0;
  virtual void visit_return_node(const ReturnNode *node) = 0;
  virtual void visit_assignment_node(const AssignmentNode *node) = 0;
  virtual void visit_var_decl_node(const VariableDeclarationNode *node) = 0;
  virtual void visit_var_init_node(const VariableInitializationNode *node) = 0;
};