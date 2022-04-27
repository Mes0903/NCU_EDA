#include <fstream>
#include <iostream>
#include <queue>
#include <regex>
#include <sstream>
#include <string>

namespace Node {
  // The type of operatrion
  enum OP_TYPE { AND,
                 OR,
                 NAND,
                 NOR,
                 NOT,
                 BUF,
                 XOR,
                 XNOR };
  // The type of wire
  enum WIRE_TYPE { NORMAL,
                   INPUT,
                   OUTPUT };

  // The structure of operation
  struct OPERATION {
    OP_TYPE type;
    uint32_t value, right, left; // The key of ROBDD.
    std::string name;
    std::vector<std::string> parameter;
  };

  // The structure of wire
  struct WIRE {
    WIRE_TYPE type;
    std::string name;
    WIRE() = default;
    WIRE(WIRE_TYPE ot, std::string on)
        : type(ot), name(on) {}
  };

  // The structure of module
  struct MODULE {
    std::vector<MODULE> mod_list;    // module may contained other modules.
    std::vector<OPERATION> op_list;    // All the operations in the module.
    std::vector<WIRE> wire_list;    // all the wires in the in the module.
    std::string parameter;    // The parameter passed into the module.
    std::string name;    // The module name.
  };
}    // namespace Node

Node::MODULE top_module;    // Only one top_module

void push_wire_param(std::string parameter_list, std::vector<Node::WIRE> &vec, Node::WIRE_TYPE type)
{
  std::string::size_type begin, end;
  std::string key = ", ";
  end = parameter_list.find(key);
  begin = 0;

  while (end != std::string::npos) {
    if (end - begin != 0) {
      vec.emplace_back(type, parameter_list.substr(begin, end - begin));
      std::cout << parameter_list.substr(begin, end - begin) << '\n';
    }
    begin = end + 2;
    end = parameter_list.find(key, begin);
  }

  if (begin != parameter_list.length()) {
    vec.emplace_back(type, parameter_list.substr(begin, end - begin));
    std::cout << parameter_list.substr(begin, end - begin) << '\n';
  }
}

void push_op_param(std::string parameter_list, std::vector<std::string> &vec)
{
  std::string::size_type begin, end;
  std::string key = ", ";
  end = parameter_list.find(key);
  begin = 0;

  while (end != std::string::npos) {
    if (end - begin != 0) {
      vec.emplace_back(parameter_list.substr(begin, end - begin));
      std::cout << parameter_list.substr(begin, end - begin) << '\n';
    }
    begin = end + 2;
    end = parameter_list.find(key, begin);
  }

  if (begin != parameter_list.length()) {
    vec.emplace_back(parameter_list.substr(begin, end - begin));
    std::cout << parameter_list.substr(begin, end - begin) << '\n';
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
  static const std::regex reg_parameter("^(.+)+");
  static std::smatch match_list, parm;
  static std::ssub_match match_obj;

  for (auto &line : article) {
    if (std::regex_match(line, match_list, reg_module)) {
      std::cout << "regex_match: module " << std::endl;
      top_module.name = match_list[1].str();
      top_module.parameter = match_list[2].str();
    }
    else if (std::regex_match(line, match_list, reg_input)) {
      std::cout << "regex_match: input" << std::endl;
      std::string parameter_list = match_list[1].str();
      push_wire_param(parameter_list, top_module.wire_list, Node::WIRE_TYPE::INPUT);
    }
    else if (std::regex_match(line, match_list, reg_output)) {
      std::cout << "regex_match: output" << std::endl;
      std::string parameter_list = match_list[1].str();
      push_wire_param(parameter_list, top_module.wire_list, Node::WIRE_TYPE::OUTPUT);
    }
    else if (std::regex_match(line, match_list, reg_wire)) {
      std::cout << "regex_match: wire" << std::endl;
      std::string parameter_list = match_list[1].str();
      push_wire_param(parameter_list, top_module.wire_list, Node::WIRE_TYPE::NORMAL);
    }
    else if (std::regex_match(line, match_list, reg_and)) {
      std::cout << "regex_match: and" << std::endl;
      std::string parameter_list = match_list[1].str();
      Node::OPERATION tmp_op;
      push_op_param(parameter_list, tmp_op.parameter);

      top_module.op_list.push_back(std::move(tmp_op));
    }
    else if (std::regex_match(line, match_list, reg_endmodule)) {
      std::cout << "regex_match: endmodule" << std::endl;
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
    "module example (i_a, i_b, i_c, o_a);",
    "input i_a, i_b, i_c;",
    "output o_a;",
    "wire l_ab, o_c;",
    "and and1(l_ab, i_a, i_b);",
    "and and2(o_c, l_ab, i_c);",
    "endmodule"
  };

  parse_line(article);
}