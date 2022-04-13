#ifndef PRE_WORK_H
#define PRE_WORK_H
#include <fstream>
#include <iostream>
#include <string>
#include "Transform.h"
#include "Type_base.h"

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
  inline void get_title(std::istream &in_file, int &latency, int &node_num, int &edge_num);

  /**
   * @brief Push all the nodes in the source file to the List.
   *
   * @param in_file The source file.
   * @param node_num The amount of the nodes.
   */
  inline void get_node(std::istream &in_file, const int node_num);

  /**
   * @brief Build the edges in the source file for all nodes in List.
   *
   * @param in_file The source file.
   * @param edge_num  The amount of the edges.
   */
  inline void build_edge(std::istream &in_file, const int edge_num);

}    // namespace Pre_work
#endif