#include "lexer.h"
#include "token.h"
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <typeinfo>

std::string read_file(const std::string &filename) {
  std::string path = std::string(TEST_DATA_DIR) + "/" + filename;
  std::ifstream ifs(path);
  if (!ifs)
    throw std::runtime_error("Failed to open file: " + path);

  return std::string(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());
}

void compare_tokenstreams(TokenStream &actual_ts, TokenStream &expected_ts) {

  ASSERT_EQ(actual_ts.size(), expected_ts.size())
      << fmt::format("Mismatch in number of tokens, actual {}, expected {}",
                     actual_ts.size(), expected_ts.size());

  while (actual_ts.peek()) {
    auto actual_token = actual_ts.consume();
    auto expected_token = expected_ts.consume();
    EXPECT_EQ(to_string(*actual_token), to_string(*expected_token))
        << fmt::format("Mismatch, actual {}, expected {}",
                       to_string(*actual_token), to_string(*expected_token));
  }
}

TEST(LexerTest, BasicProgram) {
  auto const source_file = read_file("hello_world.se");

  TokenStream actual_ts = tokenize(source_file);

  Tokens expected_tokens;
  expected_tokens.emplace_back(
      std::make_unique<DataTypeToken>(Datatype::INTEGER));
  expected_tokens.emplace_back(std::make_unique<IdentifierToken>("huvud"));
  expected_tokens.emplace_back(std::make_unique<OpenParenToken>());
  expected_tokens.emplace_back(std::make_unique<ClosedParenToken>());
  expected_tokens.emplace_back(std::make_unique<OpenSquigglyToken>());
  expected_tokens.emplace_back(
      std::make_unique<DataTypeToken>(Datatype::INTEGER));
  expected_tokens.emplace_back(
      std::make_unique<IdentifierToken>("foersta_talet"));
  expected_tokens.emplace_back(std::make_unique<AssignmentToken>());
  expected_tokens.emplace_back(std::make_unique<IntegerLiteralToken>(1337));
  expected_tokens.emplace_back(std::make_unique<TerminatorToken>());
  expected_tokens.emplace_back(std::make_unique<ReturnToken>());
  expected_tokens.emplace_back(std::make_unique<IntegerLiteralToken>(1));
  expected_tokens.emplace_back(std::make_unique<TerminatorToken>());
  expected_tokens.emplace_back(std::make_unique<ClosedSquigglyToken>());

  auto expected_ts = TokenStream{std::move(expected_tokens)};
  compare_tokenstreams(actual_ts, expected_ts);
}

TEST(LexerTest, ValidIdentifiers) {
  std::vector<std::string> valid_identifiers = {
      "onlylowercase",
      "ONLYUPPERCASE",
      "mIxEdCaSe",
      "_underscorestart",
      "_many_underscores_here_",
      "_____why_would_you_name_a_var_this",
      "myvar123456",
      "myvar123456_seveneight9",

  };

  for (const std::string &identifier : valid_identifiers) {
    TokenStream actual_ts = tokenize(identifier);

    Tokens expected_tokens;
    expected_tokens.emplace_back(std::make_unique<IdentifierToken>(identifier));

    auto expected_ts = TokenStream{std::move(expected_tokens)};
    compare_tokenstreams(actual_ts, expected_ts);
  }
}
