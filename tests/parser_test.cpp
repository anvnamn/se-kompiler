#include "parser.h"
#include "token.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(ReturnStatement, VoidReturn) {
  Tokens tokens;
  tokens.emplace_back(std::make_unique<ReturnToken>());
  tokens.emplace_back(std::make_unique<TerminatorToken>());

  auto ts = TokenStream{std::move(tokens)};
  auto root_node = parse_tokens(ts);

  auto return_node = dynamic_cast<ReturnNode *>(root_node.statements[0].get());
  ASSERT_NE(return_node, nullptr) << "Expected a ReturnNode";

  ASSERT_EQ(return_node->expression, nullptr)
      << "Expected void return (nullptr expression)";
}

TEST(ReturnStatement, IntReturn) {
  Tokens tokens;
  tokens.emplace_back(std::make_unique<ReturnToken>());
  tokens.emplace_back(std::make_unique<IntegerLiteralToken>(0));
  tokens.emplace_back(std::make_unique<TerminatorToken>());

  auto ts = TokenStream{std::move(tokens)};
  auto root_node = parse_tokens(ts);

  auto return_node = dynamic_cast<ReturnNode *>(root_node.statements[0].get());
  ASSERT_NE(return_node, nullptr) << "Expected a ReturnNode";

  ASSERT_NE(return_node->expression, nullptr) << "ReturnNode has no expression";

  auto int_expr =
      dynamic_cast<IntegerLiteralNode *>(return_node->expression.get());
  ASSERT_NE(int_expr, nullptr) << "Expected an IntegerLiteralNode";

  EXPECT_EQ(int_expr->value, 0);
}

TEST(ReturnStatement, MissingTerminator) {
  Tokens tokens;
  tokens.emplace_back(std::make_unique<ReturnToken>());
  tokens.emplace_back(std::make_unique<ClosedSquigglyToken>());

  auto ts = TokenStream{std::move(tokens)};
  try {
    auto root_node = parse_tokens(ts);
    FAIL() << "Expected std::runtime_error due to missing return expression";
  } catch (const std::runtime_error &e) {
    EXPECT_THAT(e.what(), testing::ContainsRegex("[Mm]issing terminator"));
  } catch (...) {
    FAIL() << "Expected std::runtime_error, but got a different exception type";
  }
}
