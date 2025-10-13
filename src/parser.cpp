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
  } else if (auto t = dynamic_cast<IdentifierToken *>(ts.peek())) {
    ts.consume();
    return std::make_unique<IdentifierNode>(t->name);
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

  // Return statement
  if (dynamic_cast<ReturnToken *>(ts.peek())) {
    ts.consume(); // Consume return token
    auto return_node = std::make_unique<ReturnNode>(parse_expression(ts));
    if (!dynamic_cast<TerminatorToken *>(ts.consume())) {
      throw std::runtime_error("Missing terminator after return statement");
    }
    return return_node;
  }
  // Assignment statement
  else if (dynamic_cast<IdentifierToken *>(ts.peek()) &&
           dynamic_cast<AssignmentToken *>(ts.peek(1))) {
    std::string variable_name =
        dynamic_cast<IdentifierToken *>(ts.consume())->name;
    ts.consume(); // Consume assignment token
    auto assignment_node = std::make_unique<AssignmentNode>(
        std::make_unique<IdentifierNode>(variable_name), parse_expression(ts));
    if (!dynamic_cast<TerminatorToken *>(ts.consume())) {
      throw std::runtime_error("Missing terminator after assignment statement");
    }
    return assignment_node;
  }
  // Function declaration/definition statement
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
  // Variable declaration/initialization statement
  else if (dynamic_cast<DataTypeToken *>(ts.peek()) &&
           dynamic_cast<IdentifierToken *>(ts.peek(1))) {
    Datatype variable_type = dynamic_cast<DataTypeToken *>(ts.consume())->type;
    std::string name = dynamic_cast<IdentifierToken *>(ts.consume())->name;

    // Declaration
    if (dynamic_cast<TerminatorToken *>(ts.peek())) {
      ts.consume(); // Consume terminator token
      auto declaration_node = std::make_unique<VariableDeclarationNode>(
          variable_type, std::make_unique<IdentifierNode>(name));
      return declaration_node;
    }
    // Initialization
    if (!dynamic_cast<AssignmentToken *>(ts.consume())) {
      throw std::runtime_error(
          "Failed to parse variable declaration/definition");
    }
    auto initialization_node = std::make_unique<VariableInitializationNode>(
        variable_type, std::make_unique<IdentifierNode>(name),
        parse_expression(ts));
    if (!dynamic_cast<TerminatorToken *>(ts.consume())) {
      throw std::runtime_error(
          "Missing terminator after variable initialization");
    }
    return initialization_node;
  }
  throw std::runtime_error("Unable to parse statement");
}

ScopeNode parse_tokens(TokenStream &ts) {
  std::vector<std::unique_ptr<StatementNode>> statements;
  while (ts.peek()) {
    statements.emplace_back(parse_statement(ts));
  }
  return ScopeNode(std::move(statements));
}