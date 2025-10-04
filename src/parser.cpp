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

std::vector<std::unique_ptr<ParameterNode>> parse_parameters(TokenStream &ts) {
  std::vector<std::unique_ptr<ParameterNode>> params;
  while (!dynamic_cast<ClosedParenToken *>(ts.peek())) {
    Datatype return_type = dynamic_cast<DataTypeToken *>(ts.consume())->type;
    std::string name = dynamic_cast<IdentifierToken *>(ts.consume())->name;
    params.emplace_back(std::make_unique<ParameterNode>(
        return_type, std::make_unique<IdentifierNode>(name)));
    // If next token is a comma token, consume it.
    if (dynamic_cast<CommaToken *>(ts.peek())) {
      ts.consume();
    }
  }
  ts.consume(); // Consume closed paren
  return params;
}

Program parse_tokens(TokenStream &ts) {
  std::vector<std::unique_ptr<StatementNode>> statements;

  while (ts.peek()) {
    fmt::print("Parsing {}", to_string(*ts.peek()));

    // Return node
    if (dynamic_cast<ReturnToken *>(ts.peek())) {
      ts.consume(); // Consume return token
      auto return_node = std::make_unique<ReturnNode>(parse_expression(ts));
      if (!dynamic_cast<TerminatorToken *>(ts.consume())) {
        throw std::runtime_error("Missing terminator after return statement");
      }
      statements.emplace_back(std::move(return_node));
    }
    // Function declaration
    else if (dynamic_cast<DataTypeToken *>(ts.peek()) &&
             dynamic_cast<IdentifierToken *>(ts.peek(1)) &&
             dynamic_cast<OpenParenToken *>(ts.peek(2))) {
      Datatype return_type = dynamic_cast<DataTypeToken *>(ts.consume())->type;
      std::string name = dynamic_cast<IdentifierToken *>(ts.consume())->name;
      ts.consume(); // consume open paren
      auto params = parse_parameters(ts);
      if (!dynamic_cast<TerminatorToken *>(ts.consume())) {
        throw std::runtime_error(
            "Missing terminator after function declaration");
      }
      auto funcdef_node = std::make_unique<FunctionDeclarationNode>(
          return_type, std::make_unique<IdentifierNode>(name),
          std::move(params));
      statements.emplace_back(std::move(funcdef_node));
    }
  }
  return Program(std::move(statements));
}