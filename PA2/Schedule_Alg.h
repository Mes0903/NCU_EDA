#ifndef SCHEDULE_ALG_H
#define SCHEDULE_ALG_H

#include <algorithm>
#include <queue>
#include <stack>
#include <tuple>
#include <vector>
#include "Type_base.h"

// Force Alg approach 2, Scheduled node from top to bottom, ref may be more, but less execution time
// default approach 1, sheduled all nodes each loop, ref may reduce, but more execution time
#define TOP_TO_BOTTOM

// Optmize for second Force approach
// ref may reduce, but more execution time
//#define SECOND_LAYER_OPT

namespace Type_base {
  class Node;
  enum class TYPE;
}    // namespace Type_base

/**
 * @namespace Schedule_Alg
 * The Algorithm are all in this namespace. Also, the List of Node are in this namespace.
 */
namespace Schedule_Alg {

  extern std::vector<Type_base::Node> List;    // The List of the nodes.

  /**
   * @namespace Alg_detail
   * The detailed implementation for user interface functions using.
   */
  namespace Alg_detail {

    /**
     * @brief Compute ASAP algorithm.
     */
    void Alg_ASAP();

    /**
     * @brief
     *
     * @param latency The latency of the ALAP Algorithm.
     * @return true If the scheduled successed.
     * @return false If the scheduled failed.
     */
    bool Alg_ALAP(const int latency);


    /**
     * @brief Push the node into the answer buffer and update the amount of operators.
     * @param node The target node.
     * @param Output The Answer buffer.
     */
    void push_answ(const Type_base::Node &node, std::vector<std::deque<int>> &Output);

    /**
     * @brief Get the corresponding distribution based on what the Type is.
     *
     * @param Type The type be parsed.
     * @param add_distr The adder Operation-type distribution.
     * @param multi_distr The multiplier Operation-type distribution.
     * @return The pointer of the corresponding distribution, nullptr if the Type was not normal type(add or multiply).
     */
    std::vector<double> *
    distr_case(const Type_base::TYPE &Type, std::vector<double> &add_distr, std::vector<double> &multi_distr);

    /**
     * @brief Compute the Operation-type probobality distribution.
     *
     * @param add_distr The adder Operation-type distribution.
     * @param multi_distr The multiplier Operation-type distribution.
     * @param Output The Answer buffer.
     */
    void compute_distr(std::vector<double> &add_distr, std::vector<double> &multi_distr, std::vector<std::deque<int>> &Output);


  }    // namespace Alg_detail

  /**
   * @brief Compute the Force-Directed Algorithm
   *
   * @param latency The latency of the Force Algorithm.
   * @param Output The Output Buffer.
   * @return true If the scheduled successed.
   * @return false If the scheduled failed.
   */
  bool Force(const int latency, std::vector<std::deque<int>> &Output);
}    // namespace Schedule_Alg

#endif