#include "Schedule_Alg.h"
#include "Type_base.h"

namespace Schedule_Alg {
  void Alg_detail::Alg_ASAP()
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

  bool Alg_detail::Alg_ALAP(const int latency)
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

  inline void Alg_detail::push_answ(const Type_base::Node &node, std::vector<std::deque<int>> &Output)
  {
    // Push the node into the answer buffer.
    if (node.is_normal())
      for (int i = 0, layer = node.get_tS() - 1; i < node.get_unit(); ++layer, ++i)
        Output.at(layer).push_back(node.get_label());
  };    // end push_answ function

  inline std::vector<double> *
  Alg_detail::distr_case(const Type_base::TYPE &Type, std::vector<double> &add_distr, std::vector<double> &multi_distr)
  {
    using Type_base::TYPE;

    switch (Type) {
    case TYPE::ADD: return &add_distr;
    case TYPE::MULTIPLY: return &multi_distr;
    default: return nullptr;
    }
  };

  void Alg_detail::compute_distr(std::vector<double> &add_distr, std::vector<double> &multi_distr, std::vector<std::deque<int>> &Output)
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
}    // namespace Schedule_Alg