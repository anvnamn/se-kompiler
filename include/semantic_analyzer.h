#include "node.h"
#include "symbol.h"
#include <memory>
#include <vector>

class SemanticAnalyzer {
public:
  void analyze_program(Program &ast);

private:
  std::vector<ScopeInfo> scope_stack;

  void analyze_symbols(std::unique_ptr<StatementNode> const &node);
};