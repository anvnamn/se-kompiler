#include "node.h"
#include "symbol.h"
#include <memory>
#include <vector>

class SemanticAnalyzer {
public:
  void analyze_program(ScopeNode &ast);

private:
  std::vector<ScopeInfo> scope_stack;
  void analyze_scope(ScopeNode &ast);

  void analyze_symbols(std::unique_ptr<StatementNode> const &node);
};