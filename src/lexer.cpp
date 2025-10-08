#include "lexer.h"
#include <sstream>
#include <vector>

// Data types
constexpr std::string_view INTEGER_STR = "heltal";
constexpr std::string_view VOID_STR = "tom";

// Keywords
constexpr std::string_view RETURN_STR = "returnera";

TokenStream tokenize(const std::string &src) {
  std::istringstream iss(src);
  std::string word;
  std::vector<std::string> words;

  while (iss >> word) {
    words.push_back(word);
  }

  std::vector<std::unique_ptr<Token>> tokens;

  for (auto word : words) {
    size_t pos = 0;
    while (pos < word.length()) {
      if (word.find(INTEGER_STR, pos) == 0) {
        tokens.emplace_back(std::make_unique<DataTypeToken>(Datatype::INTEGER));
        pos += INTEGER_STR.length();
      } else if (word.find(VOID_STR, pos) == 0) {
        tokens.emplace_back(std::make_unique<DataTypeToken>(Datatype::VOID));
        pos += VOID_STR.length();
      } else if (word.find(RETURN_STR, pos) == 0) {
        tokens.emplace_back(std::make_unique<ReturnToken>());
        pos += RETURN_STR.length();
      } else if (word[pos] == '=') {
        tokens.emplace_back(std::make_unique<AssignmentToken>());
        pos += 1;
      } else if (word[pos] == '(') {
        tokens.emplace_back(std::make_unique<OpenParenToken>());
        pos += 1;
      } else if (word[pos] == ')') {
        tokens.emplace_back(std::make_unique<ClosedParenToken>());
        pos += 1;
      } else if (word[pos] == '{') {
        tokens.emplace_back(std::make_unique<OpenSquigglyToken>());
        pos += 1;
      } else if (word[pos] == '}') {
        tokens.emplace_back(std::make_unique<ClosedSquigglyToken>());
        pos += 1;
      } else if (word[pos] == ';') {
        tokens.emplace_back(std::make_unique<TerminatorToken>());
        pos += 1;
      } else if (isdigit(word[pos])) {
        std::string num;
        for (size_t i = pos; i < word.length(); ++i) {
          char c = word[i];
          if (isdigit(c)) {
            num += c;
          } else {
            break;
          }
        }
        tokens.emplace_back(std::make_unique<IntegerLiteralToken>(stoi(num)));
        pos += num.length();
      } else if (isalpha(word[pos]) || word[pos] == '_') {
        std::string identifier;
        for (size_t i = pos; i < word.length(); ++i) {
          char c = word[i];
          if (isalnum(c) || c == '_') {
            identifier += c;
          } else {
            break;
          }
        }
        tokens.emplace_back(std::make_unique<IdentifierToken>(identifier));
        pos += identifier.length();
      } else {
        throw std::runtime_error("Unable to parse: " + word.substr(pos));
      }
    }
  }
  return TokenStream(std::move(tokens));
}
