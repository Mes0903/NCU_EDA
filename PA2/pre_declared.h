/**
 * @file 109201547_PA2.cpp
 * @author Mes (109201547)
 * @brief Pre-include and forward declarations for 109201547_PA2.cpp using
 * @version 0.1
 * @date 2022-04-14
 * @bug No known bugs.
 */

#ifndef PRE_DECLARED_H
#define PRE_DECLARED_H

#include <algorithm>
#include <deque>
#include <iostream>
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