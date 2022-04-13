#include "Schedule_Alg.h"
#include "Type_base.h"

namespace Schedule_Alg {
  bool Force(const int latency, std::vector<std::deque<int>> &Output)
  {
    using namespace Type_base;

    // Compute ASAP and ALAP to get tS and tL of each node.
    Alg_detail::Alg_ASAP();
    if (!Alg_detail::Alg_ALAP(latency))
      return false;

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