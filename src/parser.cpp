#include "parser.h"
#include <fmt/format.h>
#include <memory>

std::unique_ptr<ExpressionNode> parse_expression(TokenStream &ts) {
  if (auto t = dynamic_cast<IntegerLiteralToken *>(ts.peek())) {
    ts.consume();
    return std::make_unique<IntegerLiteralNode>(t->value);
  } else {
    return nullptr;
  }
}

Program parse_tokens(TokenStream &ts) {
  std::vector<std::unique_ptr<StatementNode>> statements;

  while (ts.peek()) {
    fmt::print("Parsing {}", to_string(*ts.peek()));

    if (dynamic_cast<ReturnToken *>(ts.peek())) {
      ts.consume();
      auto return_node = std::make_unique<ReturnNode>(parse_expression(ts));
      statements.emplace_back(std::move(return_node));
      if (!dynamic_cast<TerminatorToken *>(ts.consume())) {
        throw std::runtime_error("Missing terminator after return statement");
      }
    }
  }
  return Program(std::move(statements));
}