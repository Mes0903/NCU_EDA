/**
 * @file 109201547_PA2.cpp
 * @author Mes (109201547)
 * @brief Compute the Force Algorithm
 * @version 0.1
 * @date 2022-04-13
 * @bug No known bugs.
 */

#include "pre_declared.h"

/* forward declarations */
namespace Type_base {
  class Node;
}

/* End forward declarations */


int main(int argc, char *argv[])
{
  /* open the file and check if it opened successfully */
  std::ifstream in_file(argv[1]);
  if (!in_file.is_open()) {
    std::cerr << "Cannot open file: " << argv[1] << '\n';
    exit(1);
  }

  // build node
  int latency{}, node_num{}, edge_num{};
  Pre_work::get_title(in_file, latency, node_num, edge_num);
  Pre_work::get_node(in_file, node_num);
  Pre_work::build_edge(in_file, edge_num);

  // build answer buffer.
  std::vector<std::deque<int>> Output(latency);
  bool success_flag = Schedule_Alg::Force(latency, Output);
  if (!success_flag) {
    std::cerr << "failed to scheduled\n";
    return 1;
  }

  // count the amount of adder and multiplier
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
  std::ofstream out_file(argv[2]);
  if (!out_file.is_open()) {
    std::cerr << "Cannot open file: " << argv[2] << '\n';
    exit(1);
  }

  // output the file
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