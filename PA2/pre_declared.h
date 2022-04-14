#ifndef PRE_DECLARED_H
#define PRE_DECLARED_H

#include <algorithm>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include "Pre_work.h"
#include "Transform.h"
#include "Type_base.h"

/* forward declarations */
namespace Type_base {
  class Node;
}

namespace Schedule_Alg {
  bool Force(const int latency, std::vector<std::deque<int>> &Output);
}

#endif