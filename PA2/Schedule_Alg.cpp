/**
 * @file 109201547_PA2.cpp
 * @author Mes (109201547)
 * @brief The implementation of Schedule Algorithm.
 * @version 0.1
 * @date 2022-04-14
 * @bug No known bugs.
 */

#include "Schedule_Alg.h"

namespace Schedule_Alg {
  std::vector<Type_base::Node> List;    // The List of the nodes.
}

/**
 * @namespace Schedule_Alg
 * The Algorithm are all in this namespace. Also, the List of Node are in this namespace.
 */
namespace Schedule_Alg {

  /**
   * @namespace Alg_detail
   * The detailed implementation for user interface functions using.
   */
  namespace Alg_detail {

    /**
     * @brief Compute ASAP algorithm.
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
    void push_answ(const Type_base::Node &node, std::vector<std::deque<int>> &Output)
    {
      // Push the node into the answer buffer.
      if (node.is_normal())
        for (int i = 0, layer = node.get_tS() - 1; i < node.get_unit(); ++layer, ++i)
          Output.at(layer).push_back(node.get_label());
    }    // end push_answ function


    /**
     * @brief Get the corresponding distribution based on what the Type is.
     *
     * @param Type The type be parsed.
     * @param add_distr The adder Operation-type distribution.
     * @param multi_distr The multiplier Operation-type distribution.
     * @return The pointer of the corresponding distribution, nullptr if the Type was not normal type(add or multiply).
     */
    std::vector<double> *
    distr_case(const Type_base::TYPE &Type, std::vector<double> &add_distr, std::vector<double> &multi_distr)
    {
      using Type_base::TYPE;

      switch (Type) {
      case TYPE::ADD: return &add_distr;
      case TYPE::MULTIPLY: return &multi_distr;
      default: return nullptr;
      }
    }


    /**
     * @brief Compute the Operation-type probobality distribution.
     *
     * @param add_distr The adder Operation-type distribution.
     * @param multi_distr The multiplier Operation-type distribution.
     * @param Output The Answer buffer.
     */
    void compute_distr(std::vector<double> &add_distr, std::vector<double> &multi_distr, std::vector<std::deque<int>> &Output)
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
}    // namespace Schedule_Alg

namespace Schedule_Alg {

  /**
   * @brief Compute the Force-Directed Algorithm
   *
   * @param latency The latency of the Force Algorithm.
   * @param Output The Output Buffer.
   * @return true If the scheduled successed.
   * @return false If the scheduled failed.
   *
   * There are two approaches to compute and one optimization, see the macro in Schedule.h
   */
  bool Force(const int latency, std::vector<std::deque<int>> &Output)
  {
    using namespace Type_base;

    // Compute ASAP and ALAP to get tS and tL of each node.
    Alg_detail::Alg_ASAP();
    if (!Alg_detail::Alg_ALAP(latency))
      return false;

#ifdef TOP_TO_BOTTOM
    // Initialize wait_queue, push the BEGIN NOP node into the wait queue.
    std::queue<int> wait_queue;
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
#endif

    std::vector<double> *target_distr;    // Two tmp pointer point to the corresponding distribution.

#ifdef SECOND_LAYER_OPT
    std::vector<double> *second_distr;
#endif
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
    std::vector<std::tuple<int, int, double>> force_buffer;    // a tmp buffer, using in every for loop.
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
      Alg_detail::push_answ(List[target_label_s], Output);    // Push it into answer node.

      // If the node will influence the tS of its child node, the value of tS of child node should be fixed
      for (const int &parent_label : List[target_label_s].parents) {
        if (target_layer >= List[parent_label].get_tL())
          List[parent_label].set_tL(target_layer + 1);
      }
      total_force.clear();

      // Recompute the Operation-type probobality distribution.
      Alg_detail::compute_distr(add_distr, multi_distr, Output);
    }
#endif

    return true;
  }    // end Force function
}    // namespace Schedule_Alg