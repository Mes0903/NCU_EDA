#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

// cleaning std::stringstream ss
#define clear_ss(ss) \
  ss.str("");        \
  ss.clear()


/* forward declarations */
namespace Type_base {
  class Node;
}

namespace Schedule_Alg {
  extern std::vector<Type_base::Node> List;
}
/* End forward declarations */

namespace Transform {

  std::string buf;
  std::stringstream ss;

  namespace tr_detail {
    // base transformation function
    template <typename T>
    inline void transforming(T &&arg) noexcept
    {
      ss >> arg;
      clear_ss(ss);
    }

    // recursive transformation function
    template <typename Arg, typename... T>
    inline void transforming(Arg &&first, T &&...args) noexcept
    {
      ss >> first;
      transforming(std::forward<decltype(args)>(args)...);
    }
  }    // namespace tr_detail

  // user transforming function
  template <typename... T>
  void trans_input(T &&...args) noexcept
  {
    ss << buf;
    tr_detail::transforming(std::forward<decltype(args)>(args)...);
  }
}    // namespace Transform

namespace Type_base {

  // the operation type
  enum class TYPE : uint8_t {
    NOP,
    INPUT,
    ADD,
    MULTIPLY,
    OUTPUT
  };
  constexpr int UNIT[] = { 1, 1, 1, 3, 0 };    // use as a mapping from type to unit time

  class Node {
  public:
    // setter
    inline void set_unit(const int o_unit) noexcept { unit = o_unit; }
    inline void set_label(const int o_label) noexcept { label = o_label; }
    inline void set_layer(const int o_layer) noexcept { layer = o_layer; }
    inline void set_type(const TYPE o_type) noexcept { Type = o_type; }
    inline void set_tS(const int o_tS) noexcept { tS = o_tS; }
    inline void set_tL(const int o_tL) noexcept { tL = o_tL; }
    inline void set_changed(const bool flag) noexcept { changed = flag; }
    inline void set_scheduled(const bool flag) noexcept { scheduled = flag; }

    // getter
    inline const int get_unit() const noexcept { return unit; }
    inline const int get_label() const noexcept { return label; }
    inline const int get_layer() const noexcept { return layer; }
    inline const TYPE get_type() const noexcept { return Type; }
    inline const int get_tS() const noexcept { return tS; }
    inline const int get_tL() const noexcept { return tL; }
    inline const bool is_changed() const noexcept { return changed; }
    inline const bool is_scheduled() const noexcept { return scheduled; }

    // constructors
    Node() noexcept
        : unit(), label(), layer(-1), changed(false), Type(TYPE::NOP), scheduled(false), tS(), tL() {}

    Node(int label, TYPE Type) noexcept
        : unit(UNIT[static_cast<typename std::underlying_type<TYPE>::type>(Type)]), label(label), layer(), changed(false), Type(Type), scheduled(false), tS(), tL() {}

    Node(const Node &other) = delete;    // nodes are unique

    Node(Node &&other) noexcept
        : unit(other.unit), label(other.label), layer(other.layer), changed(other.changed), Type(other.Type), tS(other.tS), tL(other.tL)
    {
      parents = std::move(other.parents);
      children = std::move(other.children);
    }

    // assign operator
    Node &operator=(const Node &other) = delete;    // nodes are unique
    Node &operator=(Node &&other) noexcept
    {
      unit = other.unit, label = other.label, layer = other.layer, changed = other.changed, Type = other.Type, tS = other.tS, tL = other.tL;

      parents = std::move(other.parents);
      children = std::move(other.children);
      return *this;
    };

    ~Node() noexcept = default;

    // Parent List and Child List
    std::vector<int> parents, children;

  private:
    int unit, label, layer;
    int tS, tL;    // step number of ASAP and ALAP;
    TYPE Type;
    bool changed, scheduled;
  };
}    // namespace Type_base

namespace Pre_work {
  namespace Pre_detail {
    // map the type from character to enum class
    inline Type_base::TYPE parse_type(const char c) noexcept
    {
      using Type_base::TYPE;

      switch (c) {
      case 'i': return TYPE::INPUT;
      case '+': return TYPE::ADD;
      case '*': return TYPE::MULTIPLY;
      case 'o': return TYPE::OUTPUT;
      default: return TYPE::NOP;
      }
    }    // end parse_type
  }    // namespace Pre_detail

  // get the first three line information
  inline void get_title(std::istream &in_file, int &latency, int &node_num, int &edge_num) noexcept
  {
    using Schedule_Alg::List;
    using Transform::buf;
    using Transform::ss;
    using Transform::trans_input;

    std::getline(in_file, buf), trans_input(latency);
    std::getline(in_file, buf), trans_input(node_num);
    std::getline(in_file, buf), trans_input(edge_num);
    List.reserve(node_num + 2);
    List.emplace_back();
  }    // end get_title

  // push all the nodes in the file to the List vector
  inline void get_node(std::istream &in_file, const int node_num) noexcept
  {
    using Schedule_Alg::List;
    using Transform::buf;
    using Transform::ss;
    using Transform::trans_input;
    using Type_base::TYPE;

    int label;
    char t;
    std::stack<int> output_buf;
    TYPE Type;
    for (int i = 0; i < node_num; ++i) {
      std::getline(in_file, buf);
      trans_input(label, t);
      Type = Pre_detail::parse_type(t);
      List.emplace_back(label, Type);

      if (Type == TYPE::INPUT) {
        List[0].children.push_back(label);
        List[label].parents.push_back(0);
      }
      else if (Type == TYPE::OUTPUT) {
        output_buf.push(label);
      }
    }

    List.emplace_back(node_num + 1, TYPE::NOP);
    while (!output_buf.empty()) {
      label = output_buf.top();
      output_buf.pop();

      List[label].children.push_back(node_num + 1);
      List[node_num + 1].parents.push_back(label);
    }
  }    // end get_node

  // build the edges in the file for all nodes in List vector
  inline void build_edge(std::istream &in_file, const int edge_num) noexcept
  {
    using Schedule_Alg::List;
    using Transform::buf;
    using Transform::ss;
    using Transform::trans_input;

    int parent, child;
    for (int i = 0; i < edge_num; ++i) {
      std::getline(in_file, buf);
      trans_input(parent, child);
      List[parent].children.push_back(child);
      List[child].parents.push_back(parent);
    }
  }    // end build_edge
}    // namespace Pre_work

namespace Schedule_Alg {

  std::vector<Type_base::Node> List;

  namespace Alg_detail {
    void ASAP() noexcept
    {
      using namespace Type_base;

      std::vector<int> wait_queue, parent_queue;
      int last = List.size() - 1;
      List[0].set_scheduled(true);
      parent_queue.push_back(0);

      int tmp_step, max_step = 0;
      bool pass_flag = true;
      while (!List[last].is_scheduled()) {
        for (const int &current_parent : parent_queue) {
          // iterate over all child nodes
          for (const int &child_label : List[current_parent].children) {
            // if all of the parents of the child node have been scheduled, push it into task queue
            for (const int &parent_label : List[child_label].parents) {
              // if the parent node haven't been scheduled, check next child node
              if (!List[parent_label].is_scheduled()) {
                pass_flag = false;
                break;
              }

              // find max{tS + unit} for the number of scheduled layer
              tmp_step = List[parent_label].get_tS() + List[parent_label].get_unit();
              if (max_step < tmp_step)
                max_step = tmp_step;
            }

            if (pass_flag) {
              List[child_label].set_tS(max_step);
              List[child_label].set_scheduled(true);
              wait_queue.push_back(child_label);
            }

            max_step = 0, tmp_step = 0;
          }
        }

        parent_queue = std::move(wait_queue);
        wait_queue.clear();
        wait_queue.reserve(2 * parent_queue.size());
      }

      for (Node &node : List)
        node.set_scheduled(false);
    }    // end ASAP

    bool ALAP(const int latency) noexcept
    {
      bool success_flag = true;

      using namespace Type_base;
      std::vector<int> wait_queue, child_queue;
      int last = List.size() - 1;
      List[last].set_scheduled(true);
      List[last].set_tL(latency);
      child_queue.push_back(last);

      int tmp_step, min_step = 0;
      bool pass_flag = true;
      while (!List[0].is_scheduled()) {
        for (const int &current_child : child_queue) {
          for (const int &parent_label : List[current_child].parents) {
            for (const int &child_label : List[parent_label].children) {
              if (!List[child_label].is_scheduled()) {
                pass_flag = false;
                break;
              }

              tmp_step = List[child_label].get_tL() - List[child_label].get_unit();
              if (min_step > tmp_step)
                min_step = tmp_step;
            }

            if (min_step < 0)
              return false;

            if (pass_flag) {
              List[parent_label].set_tL(min_step);
              List[parent_label].set_scheduled(true);
              wait_queue.push_back(parent_label);
            }
          }

          child_queue = std::move(wait_queue);
          wait_queue.clear();
          wait_queue.reserve(2 * child_queue.size());
        }
      }

      for (Node &node : List)
        node.set_scheduled(false);
    }
  }    // namespace Alg_detail
}    // namespace Schedule_Alg

int main(int argc, char *argv[])
{
  /* open the file and check if it opened successfully */
  std::ifstream in_file(argv[1]);
  if (!in_file.is_open()) {
    std::cerr << "Cannot open file: " << argv[1] << '\n';
    exit(1);
  }

  using Schedule_Alg::List;
  using Transform::trans_input;
  using Type_base::Node;
  using namespace Pre_work;

  int latency{}, node_num{}, edge_num{};
  get_title(in_file, latency, node_num, edge_num);
  get_node(in_file, node_num);
  build_edge(in_file, edge_num);


  using namespace Schedule_Alg::Alg_detail;
  ASAP();

  for (const auto &node : List) {
    std::cout << node.get_label() << " Parent is : ";
    for (const auto &parent : node.parents)
      std::cout << parent << " ";
    std::cout << "\n Childs is : ";
    for (const auto &child : node.children)
      std::cout << child << " ";
    puts("");
  }

  for (const auto &node : List)
    std::cout << node.get_label() << " tS is : " << node.get_tS() << "\n";

  /* output ans to the file*/
  std::ofstream out_file(argv[2]);
  if (!out_file.is_open()) {
    std::cerr << "Cannot open file: " << argv[2] << '\n';
    exit(1);
  }


  return 0;
}