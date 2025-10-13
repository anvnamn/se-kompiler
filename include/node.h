#pragma once

#include "symbol.h"
#include "token.h"
#include <fmt/core.h>
#include <ostream>

class ASTNode {
public:
  ASTNode(const ASTNode &) = delete;
  ASTNode &operator=(const ASTNode &) = delete;
  ASTNode(ASTNode &&) = delete;
  ASTNode &operator=(ASTNode &&) = delete;

  virtual ~ASTNode() = default;

protected:
  ASTNode() = default;
  virtual void print(std::ostream &os) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const ASTNode &node) {
    node.print(os);
    return os;
  }
};

inline std::string to_string(const ASTNode &node) {
  std::ostringstream oss;
  oss << node;
  return oss.str();
}

class StatementNode : public ASTNode {};

class ExpressionNode : public ASTNode {};

class OperandNode : public ExpressionNode {};

class LiteralNode : public OperandNode {};

class IntegerLiteralNode : public LiteralNode {
public:
  int value;

  explicit IntegerLiteralNode(int value) : value(value) {}

protected:
  void print(std::ostream &os) const override {
    os << "IntegerLiteralnode: " << value;
  }
};

class ScopeNode : public StatementNode {
public:
  std::vector<std::unique_ptr<StatementNode>> statements;

  ScopeNode(std::vector<std::unique_ptr<StatementNode>> statements)
      : statements(std::move(statements)) {}

protected:
  void print(std::ostream &os) const override {
    os << "ScopeNode";
    for (auto &statement : statements) {
      os << to_string(*statement);
    }
  }
};

class IdentifierNode : public OperandNode {
public:
  std::string name;
  std::shared_ptr<VariableInfo> variable_annotation;

  explicit IdentifierNode(std::string name) : name(name) {}

protected:
  void print(std::ostream &os) const override { os << "ScopeNode: " << name; }
};

class ParameterNode : public ASTNode {
public:
  Datatype datatype;
  std::unique_ptr<IdentifierNode> name;

  ParameterNode(Datatype datatype, std::unique_ptr<IdentifierNode> name)
      : datatype(std::move(datatype)), name(std::move(name)) {}

protected:
  void print(std::ostream &os) const override {
    os << "ParameterNode: " << name;
  }
};

class FunctionDeclarationNode : public StatementNode {
public:
  Datatype returnType;
  std::unique_ptr<IdentifierNode> functionName;
  std::vector<std::unique_ptr<ParameterNode>> parameters;

  FunctionDeclarationNode(
      Datatype returnType, std::unique_ptr<IdentifierNode> functionName,
      std::vector<std::unique_ptr<ParameterNode>> parameters)
      : returnType(returnType), functionName(std::move(functionName)),
        parameters(std::move(parameters)) {}

protected:
  void print(std::ostream &os) const override {
    os << "Function declaration(" << returnType << functionName;
    for (size_t i = 0; i < parameters.size(); i++) {
      os << parameters[i];
      if (i < parameters.size() - 1) {
        os << ", ";
      }
    }
    os << "))";
  }
};

class FunctionDefinitionNode : public FunctionDeclarationNode {
public:
  std::unique_ptr<ScopeNode> body;

  FunctionDefinitionNode(Datatype returnType,
                         std::unique_ptr<IdentifierNode> functionName,
                         std::vector<std::unique_ptr<ParameterNode>> parameters,
                         std::unique_ptr<ScopeNode> body)
      : FunctionDeclarationNode(returnType, std::move(functionName),
                                std::move(parameters)),
        body(std::move(body)) {}

protected:
  void print(std::ostream &os) const override {
    os << "Function declaration(" << returnType << functionName;
    for (size_t i = 0; i < parameters.size(); i++) {
      os << parameters[i];
      if (i < parameters.size() - 1) {
        os << ", ";
      }
    }
    os << "))";
  }
};

class ReturnNode : public StatementNode {
public:
  std::unique_ptr<ExpressionNode> expression;
  explicit ReturnNode(std::unique_ptr<ExpressionNode> expression)
      : expression(std::move(expression)) {}

protected:
  void print(std::ostream &os) const override {
    if (expression) {
      os << "Return: " << expression;
    } else {
      os << "Return: void";
    }
  }
};

class AssignmentNode : public StatementNode {
public:
  std::unique_ptr<IdentifierNode> variable;
  std::unique_ptr<ExpressionNode> expression;

  AssignmentNode(std::unique_ptr<IdentifierNode> variable,
                 std::unique_ptr<ExpressionNode> expression)
      : variable(std::move(variable)), expression(std::move(expression)) {}

protected:
  void print(std::ostream &os) const override {
    os << "Assignment: " << variable << "=" << expression;
  }
};

class VariableDeclarationNode : public StatementNode {
public:
  Datatype datatype;
  std::unique_ptr<IdentifierNode> variable;

  VariableDeclarationNode(Datatype datatype,
                          std::unique_ptr<IdentifierNode> variable)
      : datatype(datatype), variable(std::move(variable)) {}

protected:
  void print(std::ostream &os) const override {
    os << "Variable declaration: " << variable;
  }
};

class VariableInitializationNode : public VariableDeclarationNode {
public:
  std::unique_ptr<ExpressionNode> expr;

  VariableInitializationNode(Datatype datatype,
                             std::unique_ptr<IdentifierNode> variable,
                             std::unique_ptr<ExpressionNode> expr)
      : VariableDeclarationNode(std::move(datatype), std::move(variable)),
        expr(std::move(expr)) {}

protected:
  void print(std::ostream &os) const override {
    os << "Variable initialization: " << variable << "=" << expr;
  }
};
