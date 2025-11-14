#include "lexer.h"
#include "parser.h"
#include "test_utils.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(ReturnStatement, VoidReturn) {
  Tokens tokens;
  tokens.emplace_back(std::make_unique<ReturnToken>());
  tokens.emplace_back(std::make_unique<TerminatorToken>());

  auto ts = TokenStream{std::move(tokens)};
  auto ast = parse_tokens(ts);

  auto return_node = dynamic_cast<ReturnNode *>(ast.statements[0].get());
  ASSERT_NE(return_node, nullptr) << "Expected a ReturnNode";

  ASSERT_EQ(return_node->expression, nullptr)
      << "Expected void return (nullptr expression)";
}

TEST(ReturnStatement, IntReturn) {
  constexpr int return_value = 1337;
  Tokens tokens;
  tokens.emplace_back(std::make_unique<ReturnToken>());
  tokens.emplace_back(std::make_unique<IntegerLiteralToken>(return_value));
  tokens.emplace_back(std::make_unique<TerminatorToken>());

  auto ts = TokenStream{std::move(tokens)};
  auto ast = parse_tokens(ts);

  ASSERT_TRUE(ast.statements.size() == 1);
  auto return_node = dynamic_cast<ReturnNode *>(ast.statements[0].get());
  ASSERT_NE(return_node, nullptr) << "Expected a ReturnNode";

  ASSERT_NE(return_node->expression, nullptr) << "ReturnNode has no expression";

  auto int_expr =
      dynamic_cast<IntegerLiteralNode *>(return_node->expression.get());
  ASSERT_NE(int_expr, nullptr) << "Expected an IntegerLiteralNode";

  EXPECT_EQ(int_expr->value, return_value);
}

TEST(ReturnStatement, MissingTerminator) {
  Tokens tokens;
  tokens.emplace_back(std::make_unique<ReturnToken>());
  tokens.emplace_back(std::make_unique<ClosedSquigglyToken>());

  auto ts = TokenStream{std::move(tokens)};
  try {
    auto ast = parse_tokens(ts);
    FAIL() << "Expected std::runtime_error due to missing return expression";
  } catch (const std::runtime_error &e) {
    EXPECT_THAT(e.what(), testing::ContainsRegex("[Mm]issing terminator"));
  } catch (...) {
    FAIL() << "Expected std::runtime_error, but got a different exception type";
  }
}

TEST(FunctionDeclaration, VoidReturnNoParams) {
  Tokens tokens;
  tokens.emplace_back(std::make_unique<DataTypeToken>(Datatype::VOID));
  tokens.emplace_back(std::make_unique<IdentifierToken>("min_funktion"));
  tokens.emplace_back(std::make_unique<OpenParenToken>());
  tokens.emplace_back(std::make_unique<ClosedParenToken>());
  tokens.emplace_back(std::make_unique<TerminatorToken>());

  auto ts = TokenStream{std::move(tokens)};
  auto ast = parse_tokens(ts);

  ASSERT_TRUE(ast.statements.size() == 1) << fmt::format(
      "Expected program with 1 statement, actual: {}", ast.statements.size());
  auto return_node =
      dynamic_cast<FunctionDeclarationNode *>(ast.statements[0].get());
  ASSERT_NE(return_node, nullptr) << "Expected a FunctionDeclarationNode";
  ASSERT_EQ(return_node->returnType, Datatype::VOID)
      << "Expected void return type";
  ASSERT_EQ(return_node->parameters.size(), 0) << "Expected no parameters";
}

TEST(FunctionDeclaration, IntReturnNoParams) {
  Tokens tokens;
  tokens.emplace_back(std::make_unique<DataTypeToken>(Datatype::INTEGER));
  tokens.emplace_back(std::make_unique<IdentifierToken>("min_heltalsfunktion"));
  tokens.emplace_back(std::make_unique<OpenParenToken>());
  tokens.emplace_back(std::make_unique<ClosedParenToken>());
  tokens.emplace_back(std::make_unique<TerminatorToken>());

  auto ts = TokenStream{std::move(tokens)};
  auto ast = parse_tokens(ts);

  ASSERT_TRUE(ast.statements.size() == 1) << fmt::format(
      "Expected program with 1 statement, actual: {}", ast.statements.size());

  auto func_decl_node =
      dynamic_cast<FunctionDeclarationNode *>(ast.statements[0].get());
  ASSERT_NE(func_decl_node, nullptr) << "Expected a FunctionDeclarationNode";
  ASSERT_EQ(func_decl_node->returnType, Datatype::INTEGER)
      << "Expected integer return type";
  ASSERT_EQ(func_decl_node->parameters.size(), 0) << "Expected no parameters";
}

TEST(FunctionDefinition, VoidReturnNoParams) {
  constexpr int func_return_value = 123;
  Tokens tokens;
  tokens.emplace_back(std::make_unique<DataTypeToken>(Datatype::VOID));
  tokens.emplace_back(std::make_unique<IdentifierToken>("min_heltalsfunktion"));
  tokens.emplace_back(std::make_unique<OpenParenToken>());
  tokens.emplace_back(std::make_unique<ClosedParenToken>());
  tokens.emplace_back(std::make_unique<OpenSquigglyToken>());
  tokens.emplace_back(std::make_unique<ReturnToken>());
  tokens.emplace_back(std::make_unique<TerminatorToken>());
  tokens.emplace_back(std::make_unique<ClosedSquigglyToken>());

  auto ts = TokenStream{std::move(tokens)};
  auto ast = parse_tokens(ts);

  ASSERT_TRUE(ast.statements.size() == 1) << fmt::format(
      "Expected program with 1 statement, actual: {}", ast.statements.size());

  auto func_def_node =
      dynamic_cast<FunctionDefinitionNode *>(ast.statements[0].get());
  ASSERT_NE(func_def_node, nullptr) << "Expected a FunctionDefinitionNode";
  ASSERT_EQ(func_def_node->returnType, Datatype::VOID)
      << "Expected void return type";
  ASSERT_EQ(func_def_node->parameters.size(), 0) << "Expected no parameters";

  auto function_body = dynamic_cast<ScopeNode *>(func_def_node->body.get());
  ASSERT_TRUE(function_body->statements.size() == 1)
      << fmt::format("Expected function body with 1 statement, actual: {}",
                     function_body->statements.size());

  auto return_node =
      dynamic_cast<ReturnNode *>(function_body->statements[0].get());
  ASSERT_NE(return_node, nullptr) << "Expected a ReturnNode";

  ASSERT_TRUE(return_node->expression == nullptr);
}

TEST(FunctionDefinition, IntReturnNoParams) {
  constexpr int func_return_value = 123;
  Tokens tokens;
  tokens.emplace_back(std::make_unique<DataTypeToken>(Datatype::INTEGER));
  tokens.emplace_back(std::make_unique<IdentifierToken>("min_heltalsfunktion"));
  tokens.emplace_back(std::make_unique<OpenParenToken>());
  tokens.emplace_back(std::make_unique<ClosedParenToken>());
  tokens.emplace_back(std::make_unique<OpenSquigglyToken>());
  tokens.emplace_back(std::make_unique<ReturnToken>());
  tokens.emplace_back(std::make_unique<IntegerLiteralToken>(func_return_value));
  tokens.emplace_back(std::make_unique<TerminatorToken>());
  tokens.emplace_back(std::make_unique<ClosedSquigglyToken>());

  auto ts = TokenStream{std::move(tokens)};
  auto ast = parse_tokens(ts);

  ASSERT_TRUE(ast.statements.size() == 1) << fmt::format(
      "Expected program with 1 statement, actual: {}", ast.statements.size());

  auto func_def_node =
      dynamic_cast<FunctionDefinitionNode *>(ast.statements[0].get());
  ASSERT_NE(func_def_node, nullptr) << "Expected a FunctionDefinitionNode";
  ASSERT_EQ(func_def_node->returnType, Datatype::INTEGER)
      << "Expected integer return type";
  ASSERT_EQ(func_def_node->parameters.size(), 0) << "Expected no parameters";

  auto function_body = dynamic_cast<ScopeNode *>(func_def_node->body.get());
  ASSERT_TRUE(function_body->statements.size() == 1)
      << fmt::format("Expected function body with 1 statement, actual: {}",
                     function_body->statements.size());

  auto return_node =
      dynamic_cast<ReturnNode *>(function_body->statements[0].get());
  ASSERT_NE(return_node, nullptr) << "Expected a ReturnNode";

  auto int_lit_node =
      dynamic_cast<IntegerLiteralNode *>(return_node->expression.get());
  ASSERT_NE(int_lit_node, nullptr) << "Expected an IntegerLiteralNode";
  ASSERT_EQ(int_lit_node->value, func_return_value);
}

TEST(VariableDeclaration, Integer) {
  Tokens tokens;
  tokens.emplace_back(std::make_unique<DataTypeToken>(Datatype::INTEGER));
  tokens.emplace_back(std::make_unique<IdentifierToken>("heltal"));
  tokens.emplace_back(std::make_unique<TerminatorToken>());
  auto ts = TokenStream{std::move(tokens)};
  auto ast = parse_tokens(ts);

  ASSERT_TRUE(ast.statements.size() == 1) << fmt::format(
      "Expected program with 1 statement, actual: {}", ast.statements.size());
  auto variable_decl_node =
      dynamic_cast<VariableDeclarationNode *>(ast.statements[0].get());
  ASSERT_NE(variable_decl_node, nullptr)
      << "Expected a VariableDeclarationNode";
  ASSERT_EQ(variable_decl_node->datatype, Datatype::INTEGER);
}

TEST(VariableInitialization, IntegerLiteral) {
  constexpr int literal_value = 123123;
  Tokens tokens;
  tokens.emplace_back(std::make_unique<DataTypeToken>(Datatype::INTEGER));
  tokens.emplace_back(std::make_unique<IdentifierToken>("en_variabel"));
  tokens.emplace_back(std::make_unique<AssignmentToken>());
  tokens.emplace_back(std::make_unique<IntegerLiteralToken>(literal_value));
  tokens.emplace_back(std::make_unique<TerminatorToken>());
  auto ts = TokenStream{std::move(tokens)};
  auto ast = parse_tokens(ts);

  ASSERT_TRUE(ast.statements.size() == 1) << fmt::format(
      "Expected program with 1 statement, actual: {}", ast.statements.size());
  auto variable_init_node =
      dynamic_cast<VariableInitializationNode *>(ast.statements[0].get());
  ASSERT_NE(variable_init_node, nullptr)
      << "Expected a VariableInitializationNode";
  ASSERT_EQ(variable_init_node->datatype, Datatype::INTEGER);

  auto expr_node =
      dynamic_cast<IntegerLiteralNode *>(variable_init_node->expr.get());
  ASSERT_NE(variable_init_node, nullptr) << "Expected an IntegerLiteralNode";
  ASSERT_EQ(expr_node->value, literal_value);
}

TEST(VariableAssignment, IntegerLiteral) {
  constexpr int literal_value = 112233;
  Tokens tokens;
  tokens.emplace_back(std::make_unique<IdentifierToken>("heltal"));
  tokens.emplace_back(std::make_unique<AssignmentToken>());
  tokens.emplace_back(std::make_unique<IntegerLiteralToken>(literal_value));
  tokens.emplace_back(std::make_unique<TerminatorToken>());
  auto ts = TokenStream{std::move(tokens)};
  auto ast = parse_tokens(ts);

  ASSERT_TRUE(ast.statements.size() == 1) << fmt::format(
      "Expected program with 1 statement, actual: {}", ast.statements.size());
  auto assignment_node =
      dynamic_cast<AssignmentNode *>(ast.statements[0].get());
  ASSERT_NE(assignment_node, nullptr) << "Expected an AssignmentNode";
  auto int_lit_node =
      dynamic_cast<IntegerLiteralNode *>(assignment_node->expression.get());
  ASSERT_NE(int_lit_node, nullptr) << "Expected an IntegerLiteralNode";
  ASSERT_EQ(int_lit_node->value, literal_value);
}

TEST(VariableAssignment, NegIntegerLiteral) {
  constexpr int literal_value = -112233;
  Tokens tokens;
  tokens.emplace_back(std::make_unique<IdentifierToken>("heltal"));
  tokens.emplace_back(std::make_unique<AssignmentToken>());
  tokens.emplace_back(std::make_unique<SubtractionToken>());
  tokens.emplace_back(
      std::make_unique<IntegerLiteralToken>(-literal_value)); // Positive value
  tokens.emplace_back(std::make_unique<TerminatorToken>());
  auto ts = TokenStream{std::move(tokens)};
  auto ast = parse_tokens(ts);

  ASSERT_TRUE(ast.statements.size() == 1) << fmt::format(
      "Expected program with 1 statement, actual: {}", ast.statements.size());
  auto assignment_node =
      dynamic_cast<AssignmentNode *>(ast.statements[0].get());
  ASSERT_NE(assignment_node, nullptr) << "Expected an AssignmentNode";
  auto int_lit_node =
      dynamic_cast<IntegerLiteralNode *>(assignment_node->expression.get());
  ASSERT_NE(int_lit_node, nullptr) << "Expected an IntegerLiteralNode";
  ASSERT_EQ(int_lit_node->value, literal_value);
}

TEST(ParseProgram, HelloWorld) {
  auto const source_file = read_test_data("hello_world.se");
  TokenStream ts = tokenize(source_file);
  auto ast = parse_tokens(ts);
}
