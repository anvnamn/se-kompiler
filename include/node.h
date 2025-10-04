#include "token.h"
#include <fmt/core.h>
#include <ostream>

class ASTNode {
public:
  virtual ~ASTNode() = default;

protected:
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

class Program : public ASTNode {
public:
  std::vector<std::unique_ptr<StatementNode>> statements;

  Program(std::vector<std::unique_ptr<StatementNode>> statements)
      : statements(std::move(statements)) {}

protected:
  void print(std::ostream &os) const override {}
};

class ExpressionNode : public ASTNode {
public:
  virtual ~ExpressionNode() = default;
};

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
    os << "Return: " << expression;
  }
};