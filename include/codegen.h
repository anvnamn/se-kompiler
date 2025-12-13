#include "node.h"
#include "visitor.h"

class Codegen : public Visitor {
public:
  std::string generate_assembly(const ScopeNode &program);

  void visit_int_literal_node(const IntegerLiteralNode *node) override;
  void visit_scope_node(const ScopeNode *node) override;
  void visit_identifier_node(const IdentifierNode *node) override;
  void visit_parameter_node(const ParameterNode *node) override;
  void visit_func_decl_node(const FunctionDeclarationNode *node) override;
  void visit_func_def_node(const FunctionDefinitionNode *node) override;
  void visit_return_node(const ReturnNode *node) override;
  void visit_assignment_node(const AssignmentNode *node) override;
  void visit_var_decl_node(const VariableDeclarationNode *node) override;
  void visit_var_init_node(const VariableInitializationNode *node) override;

private:
  std::ostringstream text, data, bss;
};
