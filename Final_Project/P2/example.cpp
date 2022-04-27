#include <fstream>
#include <iostream>
#include <queue>
#include <regex>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>

namespace Node {
  // The type of operatrion
  enum class OP_TYPE { AND,
                       OR,
                       NAND,
                       NOR,
                       NOT,
                       BUF,
                       XOR,
                       XNOR,
                       NON,
                       __SPECIAL_SYMBOL__ };
  // The type of wire
  enum class WIRE_TYPE { NORMAL,
                         INPUT,
                         OUTPUT,
                         OP_INPUT,
                         OP_OUTPUT,
                         NON };

  // The structure of wire
  struct WIRE {
    WIRE_TYPE type;
    OP_TYPE op_type;
    std::string name;
    WIRE() = default;
    WIRE(WIRE_TYPE o_t, OP_TYPE o_opt, std::string o_n)
        : type(o_t), op_type(o_opt), name(o_n) {}

    bool operator==(const WIRE &other) const { return name == other.name; }
  };

  // The structure of operation
  struct GATE {
    OP_TYPE type;
    uint32_t value, right, left;    // The key of ROBDD.
    std::string name;
    std::vector<WIRE> wires;
  };

  // The structure of module
  struct MODULE {
    std::vector<MODULE> mod_list;    // module may contained other modules.
    std::vector<GATE> op_list;    // All the operations in the module.
    std::vector<WIRE> wire_list;    // all the wires in the in the module.
    std::vector<WIRE> output_list;    // all the wires in the in the module.
    std::vector<WIRE> input_list;    // all the wires in the in the module.
    std::string parameter;    // The parameter passed into the module.
    std::string name;    // The module name.
  };
}    // namespace Node


namespace std {
  template <>
  struct hash<Node::WIRE> {
    size_t operator()(const Node::WIRE &key_wire) const
    {
      return hash<std::string>()(key_wire.name);
    }
  };
}    // namespace std

std::unordered_map<Node::WIRE, std::pair<Node::WIRE, Node::WIRE>> wire_map;
Node::MODULE top_module;    // Only one top_module

void push_wire_param(std::string parameter_list, std::vector<Node::WIRE> &vec, Node::WIRE_TYPE default_w_t, Node::OP_TYPE op_type)
{
  bool first = true;
  std::string::size_type begin, end;
  std::string key = ", ";
  end = parameter_list.find(key);
  begin = 0;

  Node::WIRE_TYPE w_type = default_w_t;

  while (end != std::string::npos) {
    if (end - begin != 0) {
      if (op_type != Node::OP_TYPE::NON) {
        if (first) {
          w_type = Node::WIRE_TYPE::OUTPUT;
          first = false;
        }
        else
          w_type = Node::WIRE_TYPE::INPUT;
      }

      vec.emplace_back(w_type, op_type, parameter_list.substr(begin, end - begin));
    }
    begin = end + 2;
    end = parameter_list.find(key, begin);
  }

  if (begin != parameter_list.length()) {
    vec.emplace_back(w_type, op_type, parameter_list.substr(begin, end - begin));
    //std::cout << parameter_list.substr(begin, end - begin) << '\n';
  }
}

/**
 * @brief Building the map of wires.
 * 
 * @param parm_vec 
 */
void build_map(std::vector<Node::WIRE> &parm_vec)
{
  const Node::WIRE &key = parm_vec[0];
  std::cout << key.name << " " << parm_vec[1].name << " " << parm_vec[2].name << '\n';
  wire_map[key] = std::make_pair(parm_vec[1], parm_vec[2]);
  std::cout << "map " << key.name << " to " << wire_map[key].first.name << " " << wire_map[key].second.name << '\n';
}

/**
 * @brief output the boolean expression
 * 
 */
void make_bool_expr()
{
  std::vector<std::string> articles;
  std::string answ_line;
  Node::OP_TYPE answ_type;

  std::stack<char> symbol_stack;
  std::queue<Node::WIRE> buf;
  Node::WIRE target;

  for (const auto output_wire : top_module.output_list) {
    buf.push(output_wire);
    while (!buf.empty()) {
      target = buf.front();
      buf.pop();
      // 有找到，可以展開
      std::cout << "finding " << target.name << '\n';
      if (wire_map.find(target) != wire_map.end()) {
        buf.push(wire_map[target].first);
        std::cout << "push " << buf.front().name << "\n";

        if (target.op_type == Node::OP_TYPE::OR) {
          answ_line += '(';
          buf.push({ Node::WIRE_TYPE::NON, Node::OP_TYPE::__SPECIAL_SYMBOL__, "__SPECIAL_SYMBOL__" });
          symbol_stack.push(')');
          symbol_stack.push('+');
        }

        buf.push(wire_map[target].second);
        std::cout << "push " << buf.front().name << "\n";
      }
      else {
        if (target.op_type == Node::OP_TYPE::__SPECIAL_SYMBOL__) {
          answ_line += symbol_stack.top();
          symbol_stack.pop();
        }
        else
          answ_line += target.name;
      }
    }

    articles.push_back(std::move(answ_line));
    answ_line.clear();
  }

  std::cout << "the final output is : ";
  for (const auto line : articles) {
    std::cout << line << '\n';
  }
}

/**
 * @brief Parse the article
 * 
 * @param article The article need to be parsed.
 */
void parse_line(std::string (&article)[7])
{
  // Regex expression for all composition.
  static const std::regex reg_module("^module\\s+(.+)\\s*\\((.+)\\);$");
  static const std::regex reg_input("^input\\s+(.+);$");
  static const std::regex reg_output("^output\\s+(.+);$");
  static const std::regex reg_wire("^wire\\s+(.+);$");
  static const std::regex reg_and("^and\\s+(.+)\\s*\\((.+)\\);$");
  static const std::regex reg_endmodule("^endmodule$");
  static std::smatch match_list, parm;
  static std::ssub_match match_obj;

  for (auto &line : article) {
    if (std::regex_match(line, match_list, reg_module)) {
      top_module.name = match_list[1].str();
      top_module.parameter = match_list[2].str();
    }
    else if (std::regex_match(line, match_list, reg_input)) {
      std::string parameter_list = match_list[1].str();
      push_wire_param(parameter_list, top_module.wire_list, Node::WIRE_TYPE::INPUT, Node::OP_TYPE::NON);
    }
    else if (std::regex_match(line, match_list, reg_output)) {
      std::string parameter_list = match_list[1].str();
      push_wire_param(parameter_list, top_module.output_list, Node::WIRE_TYPE::OUTPUT, Node::OP_TYPE::NON);
    }
    else if (std::regex_match(line, match_list, reg_wire)) {
      std::string parameter_list = match_list[1].str();
      push_wire_param(parameter_list, top_module.input_list, Node::WIRE_TYPE::NORMAL, Node::OP_TYPE::NON);
    }
    else if (std::regex_match(line, match_list, reg_and)) {
      std::string name = match_list[1].str();
      std::string parameter_list = match_list[2].str();
      Node::GATE tmp_op;
      push_wire_param(parameter_list, tmp_op.wires, Node::WIRE_TYPE::NON, Node::OP_TYPE::AND);    // parse the parameter into seperate variable
      build_map(tmp_op.wires);    // add it to the map

      top_module.op_list.push_back(std::move(tmp_op));
    }
    else if (std::regex_match(line, match_list, reg_endmodule)) {
      //std::cout << "regex_match: endmodule" << std::endl;
    }
  }
}

int main(int argc, char *argv[])
{
  std::string InputFilename = "example.v";
  /* open the file and check if it opened successfully */
  std::ifstream in_file(InputFilename);
  if (!in_file.is_open()) {
    std::cerr << "Cannot open file: " << InputFilename << '\n';
    exit(1);
  }

  // example array, this is the content of the example verilog file.
  std::string article[] = {
    "module example(a, b, c, o_a);",
    "input a, b, c;",
    "output o_a;",
    "wire o_c;",
    "and and1(o_a, a, b);",
    "and and2(o_c, a, c);",
    "endmodule"
  };

  parse_line(article);
  make_bool_expr();
}