#include "Type_base.h"

/**
 * @namespace Type_base
 * All the type for implementing Algorithm are in this namespace.
 */
namespace Type_base {

  /**
   * @brief Mapping the type from character to enum class
   *
   * @param c The type descripted by character, type in source file was descripted in character.
   * @return TYPE The corresponding type of the node.
   */
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