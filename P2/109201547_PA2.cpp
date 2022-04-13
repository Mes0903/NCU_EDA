/**
 * @file 109201547_PA2.cpp
 * @author Mes (109201547)
 * @brief Compute the Force Algorithm
 * @version 0.1
 * @date 2022-04-13
 * @bug No known bugs.
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#define SECOND_LAYER_OPT
#define TOP_TO_BOTTOM

/* forward declarations */
namespace Type_base {
  class Node;
}

namespace Schedule_Alg {
  extern std::vector<Type_base::Node> List;
}
/* End forward declarations */

/**
 * @namespace Transform
 * All function and object used to transforming data are in this namespace.
 */
namespace Transform {

  // for transformation using object
  std::string buf;
  std::stringstream ss;

  /**
   * @namespace tr_detail
   * The detailed function for user interface are in this namespace.
   */
  namespace tr_detail {
    /**
     * @brief The base of transformation function.
     * @param arg The last argument for the transformation.
     */
    template <typename T>
    inline void transforming(T &&arg)
    {
      ss >> arg;
      ss.str("");
      ss.clear();
    }

    /**
     * @brief The detail implementation of the transformation function,
     *        use recursive template parameter pack to transform the argument.
     *
     * @tparam Arg The type of the head argument in the parameter pack.
     * @tparam T The type of the parameter pack.
     * @param first The head argument in the parameter pack.
     * @param args The remain argument in the parameter pack.
     */
    template <typename Arg, typename... T>
    inline void transforming(Arg &&first, T &&...args)
    {
      ss >> first;
      transforming(std::forward<decltype(args)>(args)...);
    }
  }    // namespace tr_detail

  // user transforming function
  /**
   * @brief The interface for user to transform data from stringstream/
   *
   * @tparam T The type of the parameter pack.
   * @param args The argument used to passed in detail implemented function.
   */
  template <typename... T>
  void trans_input(T &&...args)
  {
    ss << buf;
    tr_detail::transforming(std::forward<decltype(args)>(args)...);
  }
}    // namespace Transform


/**
 * @namespace Type_base
 * All the type for implementing Algorithm are in this namespace.
 */
namespace Type_base {

  // the operation type
  enum class TYPE : uint8_t {
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
  inline TYPE parse_type(const char c)
  {
    switch (c) {
    case 'i': return TYPE::INPUT;
    case '+': return TYPE::ADD;
    case '*': return TYPE::MULTIPLY;
    case 'o': return TYPE::OUTPUT;
    default: return TYPE::DEFAULT;
    }
  }    // end parse_type function

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


/**
 * @namespace Pre_work
 * The preworks of the Algorithm are in this namespace, something like get file title, get_node, etc.
 */
namespace Pre_work {

  /**
   * @brief Get the first three line information and transform it to the corresponding object.
   *
   * @param in_file The source file.
   * @param latency The latency of the Force and ALAP Algorithm.
   * @param node_num The amount of nodes.
   * @param edge_num The amount of edges.
   */
  inline void get_title(std::istream &in_file, int &latency, int &node_num, int &edge_num)
  {
    using Schedule_Alg::List;
    using Transform::buf;
    using Transform::ss;
    using Transform::trans_input;
    using Type_base::TYPE;

    std::getline(in_file, buf), trans_input(latency);    // Transform the latency information from source file.
    std::getline(in_file, buf), trans_input(node_num);    // Transform the amount of nodes from source file.
    std::getline(in_file, buf), trans_input(edge_num);    // Transform the amount of edges from source file.
    List.reserve(node_num + 2);    // Reserve the size of the List, two more nodes are added, one for the begin NOP node, one for the end NOP node.
    List.emplace_back(0, TYPE::BEGIN);    // Push the Begin NOP node into the List.
  }    // end get_title function

  /**
   * @brief Push all the nodes in the source file to the List.
   *
   * @param in_file The source file.
   * @param node_num The amount of the nodes.
   */
  inline void get_node(std::istream &in_file, const int node_num)
  {
    using Schedule_Alg::List;
    using Transform::buf;
    using Transform::ss;
    using Transform::trans_input;
    using namespace Type_base;

    int label;    // The label of node.
    char t;    // The type in the source file, used as the parameter in parser function.
    std::stack<int> output_buf;    // The parent of End NOP node will be pushed into this stack buffer.
    TYPE Type;    // Used to store the type of node.
    for (int i = 0; i < node_num; ++i) {
      std::getline(in_file, buf);    // it will get the label and the corresponding type of the node.
      trans_input(label, t);    // Transform the data into the object
      Type = parse_type(t);    // Parse the type into the enum class type.
      List.emplace_back(label, Type);    // Push it into the node List.

      // If the type of the node is input, the nodes is the child of the Begin NOP node.
      // If the type of the node is output, the nodes is the parent of the End NOP node, store it to the stack buffer.
      if (Type == TYPE::INPUT) {
        List[0].children.push_back(label);
        List[label].parents.push_back(0);
      }
      else if (Type == TYPE::OUTPUT) {
        output_buf.push(label);
      }
    }

    // After all nodes was transformed, build the parent list of the END NOP node.
    List.emplace_back(node_num + 1, TYPE::END);
    while (!output_buf.empty()) {
      label = output_buf.top();
      output_buf.pop();

      List[label].children.push_back(node_num + 1);    // Pusth the END NOP node to the child list of the parent node.
      List[node_num + 1].parents.push_back(label);    // Push the node into the parents list of the END NOP node.
    }
  }    // end get_node function

  /**
   * @brief Build the edges in the source file for all nodes in List.
   *
   * @param in_file The source file.
   * @param edge_num  The amount of the edges.
   */
  inline void build_edge(std::istream &in_file, const int edge_num)
  {
    using Schedule_Alg::List;
    using Transform::buf;
    using Transform::ss;
    using Transform::trans_input;

    int parent, child;    // The label of parent node and child node.
    for (int i = 0; i < edge_num; ++i) {
      std::getline(in_file, buf);
      trans_input(parent, child);    // Transform the data into the object.

      // Build the edge between two node.
      List[parent].children.push_back(child);
      List[child].parents.push_back(parent);
    }
  }    // end build_edge function
}    // namespace Pre_work

/**
 * @namespace Schedule_Alg
 * The Algorithm are all in this namespace. Also, the List of Node are in this namespace.
 */
namespace Schedule_Alg {

  std::vector<Type_base::Node> List;    // The List of the nodes.

  /**
   * @namespace Alg_detail
   * The detailed implementation for user interface functions using.
   */
  namespace Alg_detail {

    /**
     * @brief Compute ASAP algorithm.
     *
     */
    void Alg_ASAP()
    {
      using namespace Type_base;
      int last = List.size() - 1;    // The label of the END NOP node.

      std::queue<int> wait_queue;    // The waiting queue, storing the node, which has been scheduled but its may child haven't.
      wait_queue.push(0);    // Push the BEGIN NOP node into the task queue.
      List[0].set_scheduled(true);
      List[0].set_waiting(true);

      int tmp_step, max_step = 0;    // The tmperory step counter and the max step storer.
      bool pass_flag = true;    // The flag of if the node pass the test.

      // When the END NOP node was scheduled, the algorithm was done.
      while (!List[last].is_scheduled()) {
        // Iterate over all the nodes within the task queue.
        while (!wait_queue.empty()) {
          const int current_label = wait_queue.front();
          wait_queue.pop();

          // Iterate over all child nodes within the child node list of the current node.
          for (const int &target_label : List[current_label].children) {
            // Iterate over all the parent nodes within the parent node list of the child nodes of the target.
            for (const int &parent_label : List[target_label].parents) {
              // If there is a parent node haven't been scheduled, the child node failed the test, keep checking next child node
              if (!List[parent_label].is_scheduled()) {
                pass_flag = false;
                break;
              }

              // Find the maximum of {tS + unit} number, which is the scheduled layer
              tmp_step = List[parent_label].get_tS() + List[parent_label].get_unit();
              if (pass_flag && max_step < tmp_step)
                max_step = tmp_step;
            }

            // If it pass the test, sheduled it on the ASAP layer
            if (pass_flag) {
              List[target_label].set_tS(max_step);
              List[target_label].set_scheduled(true);

              // If the node was not in the waiting queue, push it into wait queue.
              if (!List[target_label].is_waiting()) {
                wait_queue.push(target_label);
                List[target_label].set_waiting(true);
              }
            }

            max_step = 0, tmp_step = 0, pass_flag = true;
          }
        }
      }

      for (Node &node : List) {
        node.set_waiting(false);
        node.set_scheduled(false);
      }
    }    // end ASAP

    /**
     * @brief
     *
     * @param latency The latency of the ALAP Algorithm.
     * @return true If the scheduled successed.
     * @return false If the scheduled failed.
     */
    bool Alg_ALAP(const int latency)
    {
      using namespace Type_base;

      int last = List.size() - 1;    // The label of the END NOP node.
      List[last].set_tL(latency + 2);    // Set the layer of END NOP node on ALAP layer

      std::queue<int> wait_queue;    // The waiting queue, storing the node, which has been scheduled but its parent may haven't.
      wait_queue.push(last);    // Push the END NOP node into the task queue.
      List[last].set_scheduled(true);
      List[last].set_waiting(true);

      int tmp_step, min_step = latency + 2;    // The tmperory step counter and the min step storer.
      bool pass_flag = true;    // The flag of if the node pass the test.

      // When the BEGIN NOP node was scheduled, the algorithm was done.
      while (!List[0].is_scheduled()) {
        // Iterate over all the nodes within the task queue.
        while (!wait_queue.empty()) {
          const int current_label = wait_queue.front();
          wait_queue.pop();
          // Iterate over all parent nodes within the parent node list of the current node.
          for (const int &target_label : List[current_label].parents) {
            // Iterate over all the child nodes within the child node list of the parent nodes of the target.
            for (const int &child_label : List[target_label].children) {
              // If there is a child node haven't been scheduled, the parent node failed the test, keep checking next parent node
              if (!List[child_label].is_scheduled()) {
                pass_flag = false;
                break;
              }
              // Find the minimum of {tL - unit} number, which is the scheduled layer
              tmp_step = List[child_label].get_tL() - List[target_label].get_unit();
              if (min_step > tmp_step)
                min_step = tmp_step;
            }

            // If the min_step is less than 0, it means that it cant complete the ALAP scheduled
            if (pass_flag && (min_step < 0))
              return false;    // cant complete this scheduled

            // If it pass the test, sheduled it on the ALAP layer
            if (pass_flag) {
              List[target_label].set_tL(min_step);
              List[target_label].set_scheduled(true);

              // If the node was not in the waiting queue, push it into wait queue.
              if (!List[target_label].is_waiting()) {
                wait_queue.push(target_label);
                List[target_label].set_waiting(true);
              }
            }

            min_step = latency + 2, tmp_step = latency + 2, pass_flag = true;
          }
        }
      }

      for (Node &node : List) {
        node.set_waiting(false);

        // if the node is critical node, scheduled it
        if (node.get_type() == TYPE::INPUT) {
          node.set_tL(0);
          node.set_layer(0);
        }
        else
          node.set_scheduled(false);    // recover the node statement
      }

      return true;    // success scheduled
    }    // end ALAP

    /**
     * @brief Push the node into the answer buffer and update the amount of operators.
     * @param node The target node.
     * @param Output The Answer buffer.
     */
    inline void push_answ(const Type_base::Node &node, std::vector<std::deque<int>> &Output)
    {
      using Type_base::TYPE;
      // Push the node into the answer buffer.
      for (int i = 0, layer = node.get_tS() - 1; i < node.get_unit(); ++layer, ++i)
        Output.at(layer).push_back(node.get_label());
    };    // end push_answ function

    /**
     * @brief Get the corresponding distribution based on what the Type is.
     *
     * @param Type The type be parsed.
     * @param add_distr The adder Operation-type distribution.
     * @param multi_distr The multiplier Operation-type distribution.
     * @return The pointer of the corresponding distribution, nullptr if the Type was not normal type(add or multiply).
     */
    inline std::vector<double> *
    distr_case(const Type_base::TYPE &Type, std::vector<double> &add_distr, std::vector<double> &multi_distr)
    {
      using Type_base::TYPE;

      switch (Type) {
      case TYPE::ADD: return &add_distr;
      case TYPE::MULTIPLY: return &multi_distr;
      default: return nullptr;
      }
    };

    /**
     * @brief Compute the Operation-type probobality distribution.
     *
     * @param add_distr The adder Operation-type distribution.
     * @param multi_distr The multiplier Operation-type distribution.
     * @param Output The Answer buffer.
     */
    void
    compute_distr(std::vector<double> &add_distr,
                  std::vector<double> &multi_distr,
                  std::vector<std::deque<int>> &Output)
    {
      // Refresh the distribution.
      std::fill(add_distr.begin(), add_distr.end(), 0);
      std::fill(multi_distr.begin(), multi_distr.end(), 0);

      int local_tS, local_tL;    // The tmp tS and tL used in for loop.
      double local_probability;    // The tmp Operation probability used in for loop.
      std::vector<double> *distr;    // The pointer point to the correspond distribution.
      for (auto &node : List) {
        // If the mobility of the node is 0, and the node haven't be scheduled, the node should be scheduled since it was only one layer it can handed on.
        if (node.get_mobility() == 0 && !node.is_scheduled()) {
          node.set_layer(node.get_tS());
          node.set_scheduled(true);
          push_answ(node, Output);
        }

        local_tS = node.get_tS(), local_tL = node.get_tL();
        local_probability = node.get_probability();
        distr = distr_case(node.get_type(), add_distr, multi_distr);

        // Add the  Operation probability to the corresponding Operation-type distribution.
        if (distr) {
          for (int layer = local_tS; layer != local_tL; ++layer)
            (*distr)[layer] += local_probability;
        }
      }
    };

  }    // namespace Alg_detail

  /**
   * @brief Compute the Force-Directed Algorithm
   *
   * @param latency The latency of the Force Algorithm.
   * @param Output The Output Buffer.
   * @return true If the scheduled successed.
   * @return false If the scheduled failed.
   */
  bool Force(const int latency, std::vector<std::deque<int>> &Output)
  {
    using namespace Type_base;

    // Compute ASAP and ALAP to get tS and tL of each node.
    Alg_detail::Alg_ASAP();
    if (!Alg_detail::Alg_ALAP(latency)) {
      std::cerr << "ALAP failed\n";
      return false;
    }

#ifdef TOP_TO_BOTTOM
    // Initialize wait_queue, push the BEGIN NOP node into the wait queue.
    std::queue<int>
      wait_queue;
    wait_queue.push(0);
    List[0].set_waiting(true);
#else
    // Push all node into the wait queue.
    std::deque<int> wait_queue;
    for (Node &node : List) {
      switch (node.get_type()) {
      case TYPE::INPUT:
      case TYPE::OUTPUT:
      case TYPE::BEGIN:
        node.set_layer(0);
        node.set_scheduled(true);
        break;
      case TYPE::END:
        node.set_layer(latency + 2);
        break;
      }

      if (node.get_mobility() == 0 && !node.is_scheduled()) {
        node.set_layer(node.get_tS());
        node.set_scheduled(true);
        Alg_detail::push_answ(node, Output);    // Push it into answer node.
      }
      if (!node.is_scheduled()) {
        wait_queue.push_back(node.get_label());
        node.set_waiting(true);
      }
    }
#endif

    std::vector<double> add_distr(latency), multi_distr(latency);    // The corresponding Operation-type probobality distribution.

#ifdef TOP_TO_BOTTOM
    std::vector<std::pair<int, double>> total_force;    // The force of one node, will be clear after each for loop, the element is <layer number, total force>.
#else
    std::vector<std::tuple<int, int, double>> total_force;    // The force of all node, the element is <node label, layer number, max total force>.
#endif

    std::vector<double> *target_distr;    // Two tmp pointer point to the corresponding distribution.

#ifdef SECOND_LAYER_OPT
    std::vector<double> *second_distr;
#endif

    /**
     * @brief Compute the Total Successor force caused by all the node in child node list of the target.
     *
     * @param target_label The label of target node.
     * @param target_layer The layer of target.
     */
    auto compute_successor_force = [&add_distr, &multi_distr](const int &target_label, const int &target_layer) -> double {
      double successor_force = 0, tmp_force, buffer{};
      std::vector<double> *child_distr;

      for (const int &child_label : List[target_label].children) {
        // If the layer is bigger or equal to the child's tS, it will cause Successor force
        if (target_layer >= List[child_label].get_tS()) {
          successor_force = 0;
          const int child_tS = List[child_label].get_tS(), child_tL = List[child_label].get_tL();
          child_distr = Alg_detail::distr_case(List[child_label].get_type(), add_distr, multi_distr);    // assign the corresponding distribution of the child node.

          // Iterate over all layers.
          if (child_distr) {
            tmp_force = 0;
            for (int child_layer = child_tS; child_layer <= target_layer; ++child_layer)
              tmp_force += (*child_distr)[child_layer];

            successor_force += tmp_force * List[child_label].get_probability();
          }

          buffer += successor_force;
        }
      }    // end iterate child nodes

      return buffer;
    };

    /**
     * @brief Compute the Total Predecessor force caused by all the node in child node list of the target.
     *
     * @param target_label The label of target node.
     * @param target_layer The layer of target.
     */
    auto compute_predecessor_force = [&add_distr, &multi_distr](const int &target_label, const int &target_layer) -> double {
      double predecessor_force = 0, tmp_force, buffer{};
      std::vector<double> *parent_distr;

      // Compute the Successor force, caused by all the node in child node list of the target.
      for (const int &parent_label : List[target_label].parents) {
        // If the layer is bigger or equal to the child's tS, it will cause Successor force
        if (target_layer >= List[parent_label].get_tL()) {
          predecessor_force = 0;
          const int parent_tS = List[parent_label].get_tS();
          parent_distr = Alg_detail::distr_case(List[parent_label].get_type(), add_distr, multi_distr);    // assign the corresponding distribution of the child node.

          // Iterate over all layers.
          if (parent_distr) {
            tmp_force = 0;
            for (int parent_layer = target_layer; parent_layer <= parent_tS; ++parent_layer)
              tmp_force += (*parent_distr)[parent_layer];

            predecessor_force += tmp_force * List[parent_label].get_probability();
          }

          buffer += predecessor_force;
        }
      }    // end iterate child nodes

      return buffer;
    };


    Alg_detail::compute_distr(add_distr, multi_distr, Output);    // Compute the Operation-type distribution first time.

#ifdef TOP_TO_BOTTOM
    // If the wait queue was empty, all the node was scheduled.
    while (!wait_queue.empty()) {
      total_force.clear();    // Clear the total force
      int target_label = wait_queue.front();    // Get the label of the target node.
      wait_queue.pop();
      // Iterate over all the child node of the target node.
      for (const int &child_label : List[target_label].children) {
        // If the child node is not in the waiting queue, push it into the wait_queue.
        if (!List[child_label].is_waiting()) {
          wait_queue.push(child_label);
          List[child_label].set_waiting(true);
        }
      }

      // If the target node have been scheduled, check next node,
      // otherwise compute where it should be scheduled.
      if (!List[target_label].is_scheduled()) {
        int target_tS = List[target_label].get_tS(), target_tL = List[target_label].get_tL();    // The tmp tS and tL used in for loop.
        double self_force, successor_force, tmp_force;    // Corresponding force used in every iteration of a single node.

#ifdef SECOND_LAYER_OPT
        double second_tmp_force, second_successor_force{};
#endif

        target_distr = Alg_detail::distr_case(List[target_label].get_type(), add_distr, multi_distr);    // assign the corresponding distribution of the target node.

        // If the target node was normal type, schedule it
        if (target_distr) {
          // Compute the force of the node for ever layer
          for (int choose_layer = target_tS; choose_layer != target_tL; ++choose_layer) {
            tmp_force = (*target_distr)[choose_layer];    // The force of the layer, which the node was handed on.

            // The force of other layer didn't be chosen should be minused.
            for (int other = target_tS; other != target_tL; ++other)
              if (choose_layer != other)
                tmp_force -= (*target_distr)[other];

            self_force = tmp_force * List[target_label].get_probability();    // The self force


            // Compute the Successor force, caused by all the node in child node list of the target.
            for (const int &child_label : List[target_label].children) {
              // If the layer is bigger or equal to the child's tS, it will cause Successor force
              if (choose_layer >= List[child_label].get_tS()) {
                successor_force = 0;
                const int child_tS = List[child_label].get_tS(), child_tL = List[child_label].get_tL();
                std::vector<double> *child_distr = Alg_detail::distr_case(List[child_label].get_type(), add_distr, multi_distr);    // assign the corresponding distribution of the child node.

                // Iterate over all layers.
                if (child_distr) {
                  tmp_force = 0;
                  for (int child_layer = child_tS; child_layer <= choose_layer; ++child_layer)
                    tmp_force += (*child_distr)[child_layer];

#ifdef SECOND_LAYER_OPT
                  // Iterate all the child node within the child node list of the first child node
                  for (int child_layer = choose_layer + 1; child_layer <= child_tL; ++child_layer) {
                    for (const int &second_lable : List[child_label].children) {
                      if (child_layer >= List[second_lable].get_tS()) {
                        second_successor_force = 0;
                        const int second_tS = List[second_lable].get_tS();
                        second_distr = Alg_detail::distr_case(List[second_lable].get_type(), add_distr, multi_distr);

                        // Iterate over all layers.
                        if (second_distr) {
                          second_tmp_force = 0;
                          for (int second_layer = second_tS; second_layer <= child_layer; ++second_layer)
                            second_tmp_force += (*second_distr)[second_layer];

                          second_successor_force += second_tmp_force * List[second_lable].get_probability();
                        }

                        successor_force += second_successor_force;
                      }
                    }
                  }
#endif

                  successor_force += tmp_force * List[child_label].get_probability();
                }

                self_force += successor_force;
              }
            }    // end iterate child nodes

            // One layer of one node completed computing, keep computing force on next layer of the same node
            total_force.emplace_back(choose_layer, self_force);
          }

          // Scheduled the target node on set_layer
          const int &target_layer = std::get<0>(*std::min_element(total_force.begin(), total_force.end(), [](const std::pair<int, double> &lhs, const std::pair<int, double> &rhs) {
            return lhs.second < rhs.second;
          }));

          List[target_label].set_layer(target_layer);
          List[target_label].set_scheduled(true);
          Alg_detail::push_answ(List[target_label], Output);    // Push it into answer node.

          // If the node will influence the tS of its child node, the value of tS of child node should be fixed
          for (const int &child_label : List[target_label].children) {
            if (target_layer >= List[child_label].get_tS())
              List[child_label].set_tS(target_layer + 1);
          }

          // Recompute the Operation-type probobality distribution.
          Alg_detail::compute_distr(add_distr, multi_distr, Output);
        }
      }
    }
#else
    std::vector<std::tuple<int, int, double>> force_buffer;
    while (!wait_queue.empty()) {
      for (const int &target_label : wait_queue) {
        int target_tS = List[target_label].get_tS(), target_tL = List[target_label].get_tL();    // The tmp tS and tL used in for loop.
        double self_force, tmp_force;    // Corresponding force used in every iteration of a single node.

        target_distr = Alg_detail::distr_case(List[target_label].get_type(), add_distr, multi_distr);    // assign the corresponding distribution of the target node.

        // Compute the total force of one node.
        if (target_distr) {
          // Compute the force of the node for ever layer
          for (int choose_layer = target_tS; choose_layer != target_tL; ++choose_layer) {
            tmp_force = (*target_distr)[choose_layer];    // The force of the layer, which the node was handed on.

            // The force of other layer didn't be chosen should be minused.
            for (int other = target_tS; other != target_tL; ++other)
              if (choose_layer != other)
                tmp_force -= (*target_distr)[other];

            self_force = tmp_force * List[target_label].get_probability();    // The self force
            self_force += compute_successor_force(target_label, choose_layer);
            self_force += compute_predecessor_force(target_label, choose_layer);

            // One layer of one node completed computing, keep computing force on next layer of the same node
            force_buffer.emplace_back(target_label, choose_layer, self_force);
          }
        }

        // Scheduled the target node on set_layer
        total_force.push_back(std::move(*std::min_element(force_buffer.begin(), force_buffer.end(), [](const std::tuple<int, int, double> &lhs, const std::tuple<int, int, double> &rhs) {
          return std::get<2>(lhs) < std::get<2>(rhs);
        })));
        force_buffer.clear();
      }

      const auto target_tuple = std::move(*std::min_element(total_force.begin(), total_force.end(), [](const std::tuple<int, int, double> &lhs, const std::tuple<int, int, double> &rhs) {
        return std::get<2>(lhs) < std::get<2>(rhs);
      }));
      const int &target_label_s = std::get<0>(target_tuple), &target_layer = std::get<1>(target_tuple);

      List[target_label_s].set_layer(target_layer);
      List[target_label_s].set_scheduled(true);
      wait_queue.erase(std::remove(wait_queue.begin(), wait_queue.end(), target_label_s), wait_queue.end());
      Alg_detail::push_answ(List[target_label_s], NUM_LABEL, Output);    // Push it into answer node.

      // If the node will influence the tS of its child node, the value of tS of child node should be fixed
      for (const int &parent_label : List[target_label_s].parents) {
        if (target_layer >= List[parent_label].get_tL())
          List[parent_label].set_tL(target_layer + 1);
      }
      total_force.clear();

      // Recompute the Operation-type probobality distribution.
      Alg_detail::compute_distr(add_distr, multi_distr, NUM_LABEL, Output);
    }
#endif

    return true;
  }    // end Force function
}    // namespace Schedule_Alg

int main(int argc, char *argv[])
{
  std::string File_name = argv[1];

  /* open the file and check if it opened successfully */
  std::ifstream in_file(File_name);
  if (!in_file.is_open()) {
    std::cerr << "Cannot open file: " << argv[1] << '\n';
    exit(1);
  }

  int latency{}, node_num{}, edge_num{};
  Pre_work::get_title(in_file, latency, node_num, edge_num);
  Pre_work::get_node(in_file, node_num);
  Pre_work::build_edge(in_file, edge_num);

  std::vector<std::deque<int>> Output(latency);
  bool success_flag = Schedule_Alg::Force(latency, Output);
  if (!success_flag) {
    std::cerr << "failed to scheduled\n";
    return 1;
  }

  using Schedule_Alg::List;
  using Type_base::TYPE;
  int ADD_NUM = 0, MULTIPLY_NUM = 0;
  for (const auto &line : Output) {
    int add_cnt = 0, multi_cnt = 0;

    for (const int &label : line) {
      switch (List[label].get_type()) {
      case TYPE::ADD:
        ++add_cnt;
        if (add_cnt > ADD_NUM)
          ADD_NUM = add_cnt;
        break;

      case TYPE::MULTIPLY:
        ++multi_cnt;
        if (multi_cnt > MULTIPLY_NUM)
          MULTIPLY_NUM = multi_cnt;
      }
    }
  }

  /* output ans to the file*/
  std::ofstream out_file(File_name + ".out");
  if (!out_file.is_open()) {
    std::cerr << "Cannot open file: " << File_name << '\n';
    exit(1);
  }

  out_file << ADD_NUM << '\n'
           << MULTIPLY_NUM << '\n';

  for (auto &line : Output) {
    std::sort(line.begin(), line.end());
    while (!line.empty()) {
      out_file << line.front();
      line.pop_front();

      if (!line.empty())
        out_file << ' ';
    }

    out_file << '\n';
  }

  return 0;
}