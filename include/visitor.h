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
  virtual void visit_int_literal_node(IntegerLiteralNode *node) = 0;
  virtual void visit_scope_node(ScopeNode *node) = 0;
  virtual void visit_identifier_node(IdentifierNode *node) = 0;
  virtual void visit_parameter_node(ParameterNode *node) = 0;
  virtual void visit_func_decl_node(FunctionDeclarationNode *node) = 0;
  virtual void visit_func_def_node(FunctionDefinitionNode *node) = 0;
  virtual void visit_return_node(ReturnNode *node) = 0;
  virtual void visit_assignment_node(AssignmentNode *node) = 0;
  virtual void visit_var_decl_node(VariableDeclarationNode *node) = 0;
  virtual void visit_var_init_node(VariableInitializationNode *node) = 0;
};