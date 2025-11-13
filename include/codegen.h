#include "node.h"

class Codegen {
public:
  std::string generate_assembly(const ScopeNode &program);

private:
  void generate_scope(const ScopeNode &node,
                      const std::string &exit_label = "");

  void generate_statement(const StatementNode &node,
                          const std::string &exit_label);
  void generate_function_def(const FunctionDefinitionNode &node,
                             const std::string &exit_label);
  void generate_return_node(const ReturnNode &node,
                            const std::string &exit_label);

  void generate_variable_declaration(const VariableDeclarationNode &node);

  std::ostringstream text, data, bss;
};