#pragma once

#include <iostream>

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