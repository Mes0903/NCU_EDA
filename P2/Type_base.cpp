#include "Type_base.h"

namespace Type_base {
  TYPE parse_type(const char c)
  {
    switch (c) {
    case 'i': return TYPE::INPUT;
    case '+': return TYPE::ADD;
    case '*': return TYPE::MULTIPLY;
    case 'o': return TYPE::OUTPUT;
    default: return TYPE::DEFAULT;
    }
  }    // end parse_type function
}    // namespace Type_base