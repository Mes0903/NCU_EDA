#ifndef PRE_DECLARED_H
#define PRE_DECLARED_H

#include <algorithm>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include "Transform.h"
#include "Type_base.h"

/* forward declarations */
namespace Type_base {
  class Node;
}

namespace Schedule_Alg {
  extern std::vector<Type_base::Node> List;
}

namespace Pre_work {
  inline void get_title(std::istream &in_file, int &latency, int &node_num, int &edge_num);
  inline void get_node(std::istream &in_file, const int node_num);
  inline void build_edge(std::istream &in_file, const int edge_num);
}    // namespace Pre_work

namespace Schedule_Alg {
  bool Force(const int latency, std::vector<std::deque<int>> &Output);
}

#endif