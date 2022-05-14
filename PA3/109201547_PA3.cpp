/**
 * @file 109201547_PA3.cpp
 * @author 109201547
 * @version 0.1
 * @date 2022-05-14
 *
 * @copyright Copyright (c) 2022
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>

// the position of circuit
enum class POS { X,
                 Y };

// a cell
struct Node {
  std::string name;    // cell name
  int gain;    // cell gain
  POS pos;    // cell position
  bool lock;    // if cell is locked

  Node() = default;
  Node(std::string name, POS pos, int gain = 0, bool lock = false)
      : name(name), gain(gain), pos(pos), lock(lock) {}
};

std::map<std::string, Node> Total;    // The list of all node.
Node *X_max = nullptr, *Y_max = nullptr;    // point to the max gain node.

std::map<std::string, std::unordered_set<std::string>> Cell_List, Net_List;    // Cell List and Net List
int Bucket_size = 0;    // the number of max pin of a node.
int node_num;    // the number of nodes.

/**
 * @brief Set the max gain pointer
 * 
 */
void set_max_gain()
{
  int max_x_gain = -2 * Bucket_size, max_y_gain = -2 * Bucket_size;
  bool x_change = false, y_change = false;

  for (auto &p : Total) {
    Node &node = p.second;
    if (node.pos == POS::X) {
      if (node.gain > max_x_gain && !node.lock) {
        max_x_gain = node.gain;
        X_max = &node;
        x_change = true;
      }
    }
    else {
      if (node.gain > max_y_gain && !node.lock) {
        max_y_gain = node.gain;
        Y_max = &node;
        y_change = true;
      }
    }
  }

  if (!x_change) X_max = nullptr;    // if the pointer didn't change, means that the corresponding side has no node can be changed.
  if (!y_change) Y_max = nullptr;
}

/**
 * @brief calculate the cut size.
 * 
 * @return the cut size
 */
int cal_cut_size()
{
  int cut_size = 0;
  bool x_side, y_side;

  for (const auto &p : Net_List) {
    x_side = false, y_side = false;

    for (const auto &node : p.second) {
      if (Total[node].pos == POS::X)
        x_side = true;
      else
        y_side = true;

      // if there are to node in different side, it will cause the cut size + 1
      if (x_side && y_side) {
        ++cut_size;
        break;
      }
    }
  }

  return cut_size;
}

/**
 * @brief The Fiduccia-Mattheyses Algorithm
 * 
 */
void FM_alg()
{
  auto swap_and_add = [](Node *&target, int &f, int &t) {
    int from_side, to_side, target_from, target_to;
    target->gain = 0;

    for (const auto &net : Cell_List[target->name]) {
      target_from = 0, target_to = 0;
      from_side = 0, to_side = 0;

      for (const auto &node : Net_List[net]) {
        // calculate the F(n) and T(n).
        if (Total[node].pos == target->pos) {
          ++from_side;

          // calculate the gain of the target.
          if (node != target->name)
            ++target_from;
        }
        else {
          ++to_side;

          // calculate the gain of the target.
          if (node != target->name)
            ++target_to;
        }
      }

      if (target_from == 1) ++target->gain;
      if (target_to == 1) --target->gain;

      /* before move */
      // calculate related gain
      if (to_side == 0) {
        for (auto &find : Net_List[net]) {
          if (find == target->name)
            continue;

          if (!Total[find].lock)
            ++Total[find].gain;
        }
      }
      else if (to_side == 1) {
        for (auto &find : Net_List[net]) {
          if (find == target->name)
            continue;

          if (!Total[find].lock)
            if (Total[find].pos != target->pos) {
              --Total[find].gain;
              break;
            }
        }
      }

      /* move node */
      --from_side;
      ++to_side;

      /*after move*/
      if (from_side == 0) {
        for (auto &find : Net_List[net]) {
          if (find == target->name)
            continue;

          if (!Total[find].lock)
            --Total[find].gain;
        }
      }
      else if (from_side == 1) {
        for (auto &find : Net_List[net]) {
          if (find == target->name)
            continue;

          if (!Total[find].lock)
            if (Total[find].pos == target->pos) {
              ++Total[find].gain;
              break;
            }
        }
      }
    }

    // lock the target and changed its postion
    target->lock = true;
    if (target->pos == POS::X)
      target->pos = POS::Y;
    else
      target->pos = POS::X;

    --f;
    ++t;
  };    // end lambda

  int X_size = node_num / 2, Y_size = node_num / 2, lock_num = 0;

  while (lock_num != node_num) {
    set_max_gain();    // update the max node pointer

    // |size(A) - size(B)| < n/5, n is the number of cells
    if (static_cast<double>(std::abs((X_size - Y_size))) < static_cast<double>(node_num) / 5.0) {
      if (X_max && Y_max) {
        if (X_max->gain < 0 && Y_max->gain < 0)
          break;

        if (X_max->gain > Y_max->gain)
          swap_and_add(X_max, X_size, Y_size);
        else
          swap_and_add(Y_max, Y_size, X_size);
      }
      else if (X_max) {
        if (X_max->gain < 0)
          break;

        swap_and_add(X_max, X_size, Y_size);
      }
      else if (Y_max) {
        if (Y_max->gain < 0)
          break;

        swap_and_add(Y_max, Y_size, X_size);
      }
      else
        return;
    }
    else {
      if (X_size > Y_size)
        swap_and_add(X_max, X_size, Y_size);
      else
        swap_and_add(Y_max, Y_size, X_size);
    }

    ++lock_num;
  }
}

/**
 * @brief Initialize the Gain of cells
 */
void initialize_Gain_List()
{
  // calculate initial gain
  std::string target;
  int from_side, to_side;

  for (auto &p : Cell_List) {
    target = p.first;

    for (const auto &net : p.second) {
      from_side = 0, to_side = 0;

      // calculate F(n) and T(n).
      for (const auto &find : Net_List[net]) {
        if (find == target)
          continue;

        if (Total[target].pos == Total[find].pos)
          ++from_side;
        else
          ++to_side;
      }

      if (from_side == 1)
        ++Total[target].gain;
      if (to_side == 0)
        --Total[target].gain;
    }
  }
}

/**
 * @brief Build the cells list
 * 
 */
void initialize_Node_List()
{
  node_num = Cell_List.size();
  auto it = Cell_List.begin();
  for (int i = 0; i < node_num / 2; ++i) {
    Total["c" + std::to_string(i + 1)] = Node{ "c" + std::to_string(i + 1), POS::X };
    ++it;
  }

  for (int i = node_num / 2; i < node_num; ++i) {
    Total["c" + std::to_string(i + 1)] = Node{ "c" + std::to_string(i + 1), POS::Y };
    ++it;
  }
}

/**
 * @brief Build the Cell List and Net List from the input file
 * 
 * @param name // The Cell Label
 * @param parameter_list // The cell connect to the Label cell
 */
void push_list(std::string &name, std::string &parameter_list)
{
  std::string::size_type begin, end;
  std::string tmp;
  const std::string key = " ";
  end = parameter_list.find(key);
  begin = 0;

  while (end != std::string::npos) {
    if (end - begin != 0) {
      tmp = parameter_list.substr(begin, end - begin);
      Cell_List[tmp].emplace(name);
      Net_List[name].emplace(tmp);

      if (Cell_List[tmp].size() > Bucket_size)
        Bucket_size = Cell_List[tmp].size();
    }

    begin = end + 1;
    end = parameter_list.find(key, begin);
  }

  if (begin != parameter_list.length()) {
    tmp = parameter_list.substr(begin);
    Cell_List[tmp].emplace(name);
    Net_List[name].emplace(tmp);

    if (Cell_List[tmp].size() > Bucket_size)
      Bucket_size = Cell_List[tmp].size();
  }
};

/**
 * @brief parse the line of input.
 * 
 * @param line A line of input file.
 */
void parse_line(std::string line)
{
  // Regex expression for one line.
  static const std::regex reg_module("^\\s*NET\\s+(.+)\\s+\\{(.+)\\}\\s*$");
  static std::smatch match_list;
  std::string parameter_list, name;

  if (std::regex_match(line, match_list, reg_module)) {
    name = match_list[1].str();
    parameter_list = match_list[2].str();
    push_list(name, parameter_list);
  }
}

int main(int argc, char *argv[])
{
  /* open the file and check if it opened successfully */
  std::ifstream in_file(argv[1]);
  if (!in_file.is_open()) {
    std::cerr << "Cannot open file: " << argv[1] << '\n';
    exit(1);
  }

  /* read one line and parse it */
  std::string buf;
  while (std::getline(in_file, buf))
    parse_line(buf);

  initialize_Node_List();
  initialize_Gain_List();
  FM_alg();
  int cut_size = cal_cut_size();

  /* output ans to the file*/
  std::ofstream out_file(argv[2]);
  if (!out_file.is_open()) {
    std::cerr << "Cannot open file: " << argv[2] << '\n';
    exit(1);
  }

  out_file << "cut_size " << cut_size << "\nA\n";

  std::string name;
  for (const auto &p : Total) {
    name = p.first;
    const Node &node = p.second;

    if (node.pos == POS::X)
      out_file << p.first << '\n';
  }

  out_file << "B\n";
  for (const auto &p : Total) {
    name = p.first;
    const Node &node = p.second;

    if (node.pos == POS::Y)
      out_file << p.first << '\n';
  }
}
