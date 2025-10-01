#pragma once

#include <iostream>

enum class Datatype
{
    INTEGER
};

inline std::ostream &
operator<<(std::ostream &os, const Datatype type)
{
    switch (type)
    {
    case Datatype::INTEGER:
        return os << "Integer";
    }
    throw std::logic_error("Unknown datatype");
}