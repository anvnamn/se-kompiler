#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <vector>

enum class Datatype { INTEGER, VOID };

inline std::ostream &operator<<(std::ostream &os, const Datatype type) {
  switch (type) {
  case Datatype::INTEGER:
    return os << "Integer";
  case Datatype::VOID:
    return os << "Void";
  }
  throw std::logic_error("Tried to print unknown datatype");
}

struct VariableInfo {
  std::string name;
  Datatype type;
  int stack_offset = 0;
  bool is_global = false;
};

struct FunctionInfo {
  std::string name;
  Datatype return_type;
  std::vector<Datatype> param_types;
  bool defined = false;
};

enum class ScopeType { Function, Block, Global };

struct ScopeInfo {
  ScopeInfo(ScopeType type) : type(type){};
  ScopeType type;
  int stack_size = 0;
  std::map<std::string, std::shared_ptr<VariableInfo>> variables;
};