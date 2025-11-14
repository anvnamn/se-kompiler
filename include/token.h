#pragma once

#include "symbol.h"
#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class Token {
public:
  Token(const Token &) = delete;
  Token &operator=(const Token &) = delete;
  Token(Token &&) = delete;
  Token &operator=(Token &&) = delete;

  virtual ~Token() = default;

protected:
  Token() = default;
  virtual void print(std::ostream &os) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const Token &token) {
    token.print(os);
    return os;
  }
};

inline std::string to_string(const Token &token) {
  std::ostringstream oss;
  oss << token; // calls your friend operator<<
  return oss.str();
}

using Tokens = std::vector<std::unique_ptr<Token>>;

class AssignmentToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "AssignmentToken"; }
};

class AdditionToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "AdditionToken"; }
};

class SubtractionToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "SubtractionToken"; }
};

class MultiplicationToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "MultiplicationToken"; }
};

class DivisionToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "DivisionToken"; }
};

class OpenParenToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "OpenParenToken"; }
};

class ClosedParenToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "ClosedParenToken"; }
};

class OpenSquigglyToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "OpenSquigglyToken"; }
};

class ClosedSquigglyToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "ClosedSquigglyToken"; }
};

class CommaToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "CommaToken"; }
};

class TerminatorToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "TerminatorToken"; }
};

class LiteralToken : public Token {};

class ReturnToken : public Token {
protected:
  void print(std::ostream &os) const override { os << "ReturnToken"; }
};

class IntegerLiteralToken : public LiteralToken {
public:
  int value;

  explicit IntegerLiteralToken(int value) : value{value} {}

protected:
  void print(std::ostream &os) const override {
    os << "IntegerLiteralToken:" << value;
  }
};

class DataTypeToken : public Token {
public:
  Datatype type;

  explicit DataTypeToken(Datatype type) : type{type} {}

protected:
  void print(std::ostream &os) const override {
    os << "DatatypeToken: " << type;
  }
};

class IdentifierToken : public Token {
public:
  std::string name;

  IdentifierToken(std::string name) : name{name} {}

protected:
  void print(std::ostream &os) const override {
    os << "Identifiertoken: " << name;
  }
};

class TokenStream {

  std::vector<std::unique_ptr<Token>> tokens;
  size_t current = 0;

public:
  explicit TokenStream(std::vector<std::unique_ptr<Token>> tokens)
      : tokens(std::move(tokens)) {}

  size_t size() const { return tokens.size(); }

  Token *peek() {
    return current < tokens.size() ? tokens[current].get() : nullptr;
  }

  Token *peek(int ahead) {
    return current + ahead < tokens.size() ? tokens[current + ahead].get()
                                           : nullptr;
  }

  Token *consume() {
    return current < tokens.size() ? tokens[current++].get() : nullptr;
  }
};