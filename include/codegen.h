#include "node.h"
#include "visitor.h"

class Codegen : public Visitor {
public:
  std::string generate_assembly(ScopeNode &program);

  void visit_int_literal_node(IntegerLiteralNode *node) override;
  void visit_scope_node(ScopeNode *node) override;
  void visit_identifier_node(IdentifierNode *node) override;
  void visit_parameter_node(ParameterNode *node) override;
  void visit_func_decl_node(FunctionDeclarationNode *node) override;
  void visit_func_def_node(FunctionDefinitionNode *node) override;
  void visit_return_node(ReturnNode *node) override;
  void visit_assignment_node(AssignmentNode *node) override;
  void visit_var_decl_node(VariableDeclarationNode *node) override;
  void visit_var_init_node(VariableInitializationNode *node) override;

private:
  std::ostringstream text, data, bss;
};
