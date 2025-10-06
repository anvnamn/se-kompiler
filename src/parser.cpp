#include "parser.h"
#include <fmt/format.h>
#include <memory>

std::unique_ptr<ExpressionNode> parse_expression(TokenStream &ts);
std::vector<std::unique_ptr<ParameterNode>> parse_parameters(TokenStream &ts);
std::unique_ptr<ScopeNode> parse_scope(TokenStream &ts);
std::unique_ptr<StatementNode> parse_statement(TokenStream &ts);

std::unique_ptr<ExpressionNode> parse_expression(TokenStream &ts) {
  if (auto t = dynamic_cast<IntegerLiteralToken *>(ts.peek())) {
    ts.consume();
    return std::make_unique<IntegerLiteralNode>(t->value);
  }
  return nullptr;
}

std::vector<std::unique_ptr<ParameterNode>> parse_parameters(TokenStream &ts) {
  std::vector<std::unique_ptr<ParameterNode>> params;
  ts.consume(); // consume open paren
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

std::unique_ptr<ScopeNode> parse_scope(TokenStream &ts) {
  std::vector<std::unique_ptr<StatementNode>> statements;
  if (!dynamic_cast<OpenSquigglyToken *>(ts.consume())) {
    throw std::runtime_error("Expected open squiggly at start of scope");
  }
  while (ts.peek()) {
    if (dynamic_cast<ClosedSquigglyToken *>(ts.peek())) {
      ts.consume(); // Consume closed squiggly
      return std::make_unique<ScopeNode>(std::move(statements));
    }
    statements.emplace_back(parse_statement(ts));
  }
  throw std::runtime_error("Reached end of program before end of scope");
}

std::unique_ptr<StatementNode> parse_statement(TokenStream &ts) {

  // Return node
  if (dynamic_cast<ReturnToken *>(ts.peek())) {
    ts.consume(); // Consume return token
    auto return_node = std::make_unique<ReturnNode>(parse_expression(ts));
    if (!dynamic_cast<TerminatorToken *>(ts.consume())) {
      throw std::runtime_error("Missing terminator after return statement");
    }
    return return_node;
  }
  // Function declaration/definition
  else if (dynamic_cast<DataTypeToken *>(ts.peek()) &&
           dynamic_cast<IdentifierToken *>(ts.peek(1)) &&
           dynamic_cast<OpenParenToken *>(ts.peek(2))) {
    Datatype return_type = dynamic_cast<DataTypeToken *>(ts.consume())->type;
    std::string name = dynamic_cast<IdentifierToken *>(ts.consume())->name;
    auto params = parse_parameters(ts);
    if (dynamic_cast<TerminatorToken *>(ts.peek())) {
      ts.consume(); // Consume terminator token
      return std::make_unique<FunctionDeclarationNode>(
          return_type, std::make_unique<IdentifierNode>(name),
          std::move(params));
    } else if (dynamic_cast<OpenSquigglyToken *>(ts.peek())) {
      return std::make_unique<FunctionDefinitionNode>(
          return_type, std::make_unique<IdentifierNode>(name),
          std::move(params), std::move(parse_scope(ts)));
    }
  }
  throw std::runtime_error("Unable to parse statement");
}

Program parse_tokens(TokenStream &ts) {
  std::vector<std::unique_ptr<StatementNode>> statements;
  while (ts.peek()) {
    statements.emplace_back(parse_statement(ts));
  }
  return Program(std::move(statements));
}