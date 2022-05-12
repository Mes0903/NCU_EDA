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
  int label;
  POS pos;
  bool lock, cal;
  Node *prev, *next;

  Node() = default;
  Node(std::string name, int label, POS pos, bool lock = false, bool cal = false, Node *prev = nullptr, Node *next = nullptr)
      : name(name), label(label), pos(pos), lock(lock), cal(cal), prev(prev), next(next) {}

  Node &operator=(const Node &other)
  {
    name = other.name;
    label = other.label;
    pos = other.pos;
    lock = other.lock;
    cal = other.cal;
    prev = other.prev;
    next = other.next;
  }

  bool operator==(const Node &other) { return label == other.label; }
  bool operator==(std::string target) { return name == target; }
};

std::vector<Node *> X, Y;
std::map<std::string, Node> Total;    // Gain Bucket
Node **X_max = nullptr, **Y_max = nullptr;

std::map<std::string, std::unordered_set<std::string>> Cell_List, Net_List;
int max_net = 0;    // -p(i) <= g(i) <= p(i)
int cut_size;

Node **find_list(Node *&head, Node &node)
{
  Node **indirect = &head;
  while (*indirect && *indirect == &node)
    indirect = &(*indirect)->next;

  return indirect;
}

void swap_node()
{
}

void append_bucket(Node *&head, Node &node)
{
  Node **indirect = &head;
  while (*indirect)
    indirect = &(*indirect)->next;

  *indirect = &node;
}

void set_max_gain()
{
  for (int i = 2 * max_net; i >= 0; --i) {
    if (X[i] != nullptr) {
      X_max = &X[i];
      break;
    }
  }

  for (int i = 2 * max_net; i >= 0; --i) {
    if (Y[i] != nullptr) {
      Y_max = &Y[i];
      break;
    }
  }
}

void initialize_Gain_List()
{
  // calculate initial gain
  int gain, index;
  std::string target;

  for (auto &p : Cell_List) {
    gain = 0, index = 0;
    target = p.first;

    for (const auto &net : p.second) {
      for (const auto &find : Net_List[target]) {
        if (find == target)
          continue;

        if (Total[target].pos == Total[find].pos)
          --gain;
        else
          ++gain;
      }
    }

    // transforming index
    index = gain + max_net;
    if (Total[target].pos == POS::X)
      append_bucket(X[index], Total[target]);
    else
      append_bucket(Y[index], Total[target]);
  }
}

void initialize_Node_List(const int line)
{
  auto it = Cell_List.begin();
  for (int i = 0; i < line / 2 - 1; ++i) {
    Total["c" + std::to_string(i)] = Node{ (*it).first, i, POS::X };
    ++it;
  }

  for (int i = line / 2 - 1; i < line; ++i) {
    Total["c" + std::to_string(i)] = Node{ (*it).first, i, POS::X };
    ++it;
  }
}

void initialize_Bucket(const int line)
{
  X.reserve(2 * max_net + 1);
  Y.reserve(2 * max_net + 1);

  for (int i = 0; i < 2 * max_net + 1; ++i) {
    X.emplace_back(nullptr);
    Y.emplace_back(nullptr);
  }
}

void push_list(const std::string &name, const std::string &parameter_list)
{
  std::string::size_type begin, end;
  std::string tmp;
  const std::string key = " ";
  end = parameter_list.find(key);
  begin = 0;

  while (end != std::string::npos) {
    if (end - begin != 0) {
      tmp = parameter_list.substr(begin, end - begin);
      Cell_List[tmp].insert(name);
      Net_List[name].insert(std::move(tmp));

      if (Cell_List[tmp].size() > max_net)
        max_net = Cell_List[tmp].size();
    }

    begin = end + 1;
    end = parameter_list.find(key, begin);
  }

  if (begin != parameter_list.length()) {
    tmp = parameter_list.substr(begin);
    Cell_List[tmp].insert(name);
    Net_List[name].insert(std::move(tmp));
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
  int line = 0;
  std::string buf;
  while (std::getline(in_file, buf)) {
    parse_line(buf);
    ++line;
  }

  initialize_Bucket(line);
  initialize_Node_List(line);
  initialize_Gain_List();
}