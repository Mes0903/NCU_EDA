/**
 * @file 109201547_PA3.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-05-11
 *
 * @copyright Copyright (c) 2022
 *
 * 1. 讀檔
 * 2. 建立 Cell_List、Bucket_List
 * 3. 分區塊 X、Y (初始分割)
 * 4. 計算初始 Gain
 * 5. 建立 Gain Bucket List
 * 6.
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#define clear_ss(ss) \
  ss.str("");        \
  ss.clear()

enum class POS { X,
                 Y };

struct Node {
  std::string name;
  int gain;
  POS pos;
  bool lock, cal;
  Node *prev, *next;

  Node() = default;
  Node(std::string name, int label, POS pos, int gain = 0, bool lock = false, bool cal = false, Node *prev = nullptr, Node *next = nullptr)
      : name(name), pos(pos), gain(gain), lock(lock), cal(cal), prev(prev), next(next) {}

  bool operator==(std::string target) { return name == target; }
};

std::vector<Node *> X, Y;
std::map<std::string, Node> Total;    // Gain Bucket
Node **X_max = nullptr, **Y_max = nullptr;

std::map<std::string, std::unordered_set<std::string>> Cell_List, Net_List;
int Bucket_size = 0;    // -p(i) <= g(i) <= p(i)
int node_num, net_num;
int cut_size = 0;

void print_bucket()
{
  puts("X");
  for (auto ptr : X) {
    while (ptr) {
      std::cout << ptr->name << " ";
      ptr = ptr->next;
    }
    puts("");
  }
  puts("Y");
  for (auto ptr : Y) {
    while (ptr) {
      std::cout << ptr->name << " ";
      ptr = ptr->next;
    }
    puts("");
  }
}

void print_List()
{
  for (const auto p : Cell_List) {
    std::cout << p.first << " ";
    for (const auto net : p.second) {
      std::cout << net << " ";
    }
    puts("");
  }

  for (const auto p : Net_List) {
    std::cout << p.first << " ";
    for (const auto node : p.second) {
      std::cout << node << " ";
    }
    puts("");
  }
}

void append_bucket(Node **head, Node *node)
{
  Node **indirect = head;
  while (*indirect)
    indirect = &((*indirect)->next);

  *indirect = node;
}

void set_max_gain()
{
  Node *direct;

  for (int i = 2 * Bucket_size - 2; i >= 0; --i) {
    direct = X.at(i);
    while (direct) {
      if (!direct->lock)
        break;

      direct = direct->next;
    }
    if (direct && !direct->lock) {
      X_max = &X.at(i);
      break;
    }
  }

  for (int i = 2 * Bucket_size - 2; i >= 0; --i) {
    direct = Y.at(i);

    while (direct) {
      if (!direct->lock)
        break;

      direct = direct->next;
    }

    if (direct && !direct->lock) {
      Y_max = &Y.at(i);
      break;
    }
  }
}

void initialize_cut_size()
{
  std::string target;

  for (auto &p : Cell_List) {
    target = p.first;
    Total[target].cal = true;

    for (const auto &net : p.second) {
      for (const auto &find : Net_List[target]) {
        if (find == target)
          continue;

        if ((Total[target].pos != Total[find].pos) && !Total[find].cal)
          ++cut_size;
      }
    }
  }
}

void FM_alg()
{
  auto swap_and_add = [](Node *&target) {
    int prev_index, index;
    Node **indirect, **head;
    (*target).lock = true;

    // calculate new gain
    for (const auto &net : Cell_List[target->name]) {
      for (auto &find : Net_List[net]) {
        if (target->name == find)
          continue;

        if (Total[find].lock)
          continue;

        prev_index = Total[find].gain + Bucket_size - 1;

        if (Total[target->name].pos == Total[find].pos) {
          --cut_size;
          Total[target->name].gain -= 2;
          Total[find].gain -= 2;
        }
        else {
          ++cut_size;
          Total[target->name].gain += 2;
          Total[find].gain += 2;
        }

        // add the related node to the gain bucket
        index = Total[find].gain + Bucket_size - 1;
        std::cout << "index: " << index << " ";
        std::cout << "gain: " << Total[find].gain << " ";
        std::cout << "name: " << Total[find].name << "\n";

        if (Total[find].pos == POS::X) {
          indirect = &X.at(prev_index);
          head = &X.at(index);
        }
        else {
          indirect = &Y.at(prev_index);
          head = &Y.at(index);
        }

        // delete it in the bucket
        while (*indirect && *indirect != &Total[find])
          indirect = &((*indirect)->next);

        *indirect = Total[find].next;
        Total[find].next = nullptr;

        append_bucket(head, &Total[find]);
        print_bucket();
      }
    }

    // add to the gain bucket
    index = Total[target->name].gain + Bucket_size - 1;
    if (Total[target->name].pos == POS::X)
      append_bucket(&X.at(index), target);
    else
      append_bucket(&Y.at(index), target);
  };

  // the first swap
  Node *target_X, *target_Y, *direct;
  int X_size = 0, Y_size = 0, lock_num = 0;

  while (lock_num != node_num) {
    std::cout << "lock_num: " << lock_num << " " << node_num << '\n';
    set_max_gain();

    for (Node *ptr : X) {
      if (ptr) {
        direct = ptr->next;
        while (direct) {
          puts("c");
          ++X_size;

          if (direct->lock && direct->cal) {
            direct->cal = false;
            ++lock_num;
          }

          direct = direct->next;
        }
      }
    }

    for (Node *ptr : Y) {
      if (ptr) {
        direct = ptr->next;
        while (direct) {
          puts("d");
          ++Y_size;

          if (direct && direct->lock && direct->cal) {
            direct->cal = false;
            ++lock_num;
          }

          direct = direct->next;
        }
      }
    }

    target_X = *X_max;
    target_Y = *Y_max;

    if (std::abs(X_size - Y_size) < node_num / 5) {
      if ((**X_max).gain > (**Y_max).gain) {
        *X_max = (*X_max)->next->next;
        target_X->next = nullptr;
        target_X->pos = POS::Y;
        swap_and_add(target_X);
      }
      else {
        *Y_max = (*Y_max)->next->next;
        target_Y->next = nullptr;
        target_Y->pos = POS::X;
        swap_and_add(target_Y);
      }
    }
    else {
      if (X_size > Y_size) {
        *X_max = (*X_max)->next->next;
        target_X->next = nullptr;
        target_X->pos = POS::Y;
        swap_and_add(target_X);
      }
      else {
        *Y_max = (*Y_max)->next->next;
        target_Y->next = nullptr;
        target_Y->pos = POS::X;
        swap_and_add(target_Y);
      }
    }
  }
}

void initialize_Gain_List()
{
  // calculate initial gain
  std::string target;
  for (auto &p : Cell_List) {
    target = p.first;

    for (const auto &net : p.second) {
      for (const auto &find : Net_List[target]) {
        if (find == target)
          continue;

        if (Total[target].pos == Total[find].pos)
          --Total[target].gain;
        else
          ++Total[target].gain;
      }
    }

    // transforming index
    int index = Total[target].gain + Bucket_size - 1;
    if (Total[target].pos == POS::X) {
      append_bucket(&X.at(index), &Total[target]);
    }
    else {
      append_bucket(&Y.at(index), &Total[target]);
    }
  }
}

void initialize_Node_List()
{
  node_num = Cell_List.size();
  auto it = Cell_List.begin();
  for (int i = 0; i < node_num / 2; ++i) {
    Total["c" + std::to_string(i + 1)] = Node{ "c" + std::to_string(i + 1), i, POS::X };
    ++it;
  }

  for (int i = node_num / 2; i < node_num; ++i) {
    Total["c" + std::to_string(i + 1)] = Node{ "c" + std::to_string(i + 1), i, POS::Y };
    ++it;
  }

  std::cout << "Node List:";
  for (const auto &[p, node] : Total) {
    std::cout << node.name << " ";
  }
  puts("");
}

void initialize_Bucket()
{
  X.reserve(2 * Bucket_size - 1);
  Y.reserve(2 * Bucket_size - 1);
  std::cout << "X size is : " << 2 * Bucket_size - 1 << '\n';

  for (int i = 0; i < 2 * Bucket_size - 1; ++i) {
    X.emplace_back(nullptr);
    Y.emplace_back(nullptr);
  }
}

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
};

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
  std::ifstream in_file("case0");
  if (!in_file.is_open()) {
    std::cerr << "Cannot open file: " << argv[1] << '\n';
    exit(1);
  }

  /* read one line and parse it */
  std::string buf;
  while (std::getline(in_file, buf))
    parse_line(buf);


  initialize_Bucket();
  initialize_Node_List();
  initialize_Gain_List();
  initialize_cut_size();
  print_List();
  puts("main");
  print_bucket();
  FM_alg();

  /* output ans to the file*/
  std::ofstream out_file("case0_out");
  if (!out_file.is_open()) {
    std::cerr << "Cannot open file: " << argv[2] << '\n';
    exit(1);
  }

  out_file << "cut_size " << cut_size << "A\n";
  Node **indirect;
  for (Node *ptr : X) {
    indirect = &ptr->next;
    while (*indirect) {
      out_file << (*indirect)->name << '\n';
      indirect = &((*indirect)->next);
    }
  }

  out_file << "B\n";
  for (Node *ptr : Y) {
    indirect = &ptr->next;
    while (*indirect) {
      out_file << (*indirect)->name << '\n';
      indirect = &((*indirect)->next);
    }
  }
}