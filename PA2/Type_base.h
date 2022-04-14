#ifndef TYPE_BASE_H
#define TYPE_BASE_H
#include <vector>

/**
 * @namespace Type_base
 * All the type for implementing Algorithm are in this namespace.
 */
namespace Type_base {

  // the operation type
  enum class TYPE {
    BEGIN,
    INPUT,
    ADD,
    MULTIPLY,
    OUTPUT,
    END,
    DEFAULT
  };
  constexpr int UNIT[] = { 0, 1, 1, 3, 1, 1, -1 };    // use as a map from type to unit time

  /**
   * @brief Mapping the type from character to enum class
   *
   * @param c The type descripted by character, type in source file was descripted in character.
   * @return TYPE The corresponding type of the node.
   */
  TYPE parse_type(const char c);

  /**
   * The type of the element in the List. The data structure of the List is Doubly Linked List, implemented by vector with index.
   *
   * Node has two pointer list, one point to the parent node list, one point to the child node list.
   *
   * There are several member in nodes:
   * unit: integer, The unit time of the node.
   * label: integer, The label of the node. Used to implemented Linked List.
   * layer: integer, The layer of the node in Force Algorithm.
   * tS: integer, The layer of the node in ASAP Algorithm.
   * tL: integer, The layer of the node in ALAP Algorithm.
   * Type: enum, The operation type of the node.
   * waiting: bool, The flag of whether the node was in waiting list.
   * scheduled: bool, The flag of whether of the node has been scheduled.
   */
  class Node {
  public:
    // setter
    inline void set_layer(const int o_layer) { layer = o_layer; }
    inline void set_tS(const int o_tS) { tS = o_tS; }
    inline void set_tL(const int o_tL) { tL = o_tL; }
    inline void set_waiting(const bool flag) { waiting = flag; }
    inline void set_scheduled(const bool flag) { scheduled = flag; }

    // getter
    inline const int get_unit() const { return unit; }
    inline const int get_label() const { return label; }
    inline const int get_layer() const { return layer; }
    inline const TYPE get_type() const { return Type; }
    inline const int get_tS() const { return tS; }
    inline const int get_tL() const { return tL; }
    inline const bool is_waiting() const { return waiting; }
    inline const bool is_scheduled() const { return scheduled; }

    // counting getter
    inline const int get_mobility() const { return tL - tS; }
    inline const double get_probability() const { return 1.0 / static_cast<double>(tL - tS + 1); }

    // check if the node is ADD or MULTIPLY type
    inline const bool is_normal() const { return Type == TYPE::ADD || Type == TYPE::MULTIPLY; }

    // constructors
    Node() = delete;

    Node(int label, TYPE Type)
        : unit(UNIT[static_cast<typename std::underlying_type<TYPE>::type>(Type)]), label(label), layer(), tS(), tL(), Type(Type), waiting(false), scheduled(false) {}

    Node(const Node &other) = delete;    // nodes are unique, thus no copy constructor

    Node(Node &&other)
        : unit(other.unit), label(other.label), layer(other.layer), tS(other.tS), tL(other.tL), Type(other.Type), waiting(other.waiting), scheduled(other.scheduled)
    {
      parents = std::move(other.parents);
      children = std::move(other.children);
    }

    // assign operator
    Node &operator=(const Node &other) = delete;    // nodes are unique, this no copy assignment
    Node &operator=(Node &&other)
    {
      unit = other.unit, label = other.label, layer = other.layer, Type = other.Type, tS = other.tS, tL = other.tL, waiting = other.waiting, scheduled = other.scheduled;

      parents = std::move(other.parents);
      children = std::move(other.children);
      return *this;
    };

    ~Node() = default;

    // Parent nodes List and Child nodes List
    std::vector<int> parents, children;

  private:
    int unit, label, layer;    // unit time, lable number, layer number.
    int tS, tL;    // The steps amount of ASAP and ALAP;
    TYPE Type;    // The type of this node
    bool waiting, scheduled;    // if the node in the waiting statement, if the node has been scheduled.
  };
}    // namespace Type_base
#endif