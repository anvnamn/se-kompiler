#include "parser.h"
#include "token.h"
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
  Tokens tokens;
  tokens.emplace_back(std::make_unique<ReturnToken>());
  tokens.emplace_back(std::make_unique<IntegerLiteralToken>(1337));
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

  EXPECT_EQ(int_expr->value, 1337);
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

TEST(FunctionDeclaration, VoidReturnNoArgs) {
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

TEST(FunctionDeclaration, IntReturnNoArgs) {
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
  auto return_node =
      dynamic_cast<FunctionDeclarationNode *>(ast.statements[0].get());
  ASSERT_NE(return_node, nullptr) << "Expected a FunctionDeclarationNode";
  ASSERT_EQ(return_node->returnType, Datatype::INTEGER)
      << "Expected integer return type";
  ASSERT_EQ(return_node->parameters.size(), 0) << "Expected no parameters";
}
