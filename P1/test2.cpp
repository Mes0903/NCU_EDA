#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#define clear_ss(ss) \
  ss.str("");        \
  ss.clear()


namespace detail {

  struct WIRE {
    uint32_t addr;
    std::string name, __type;

    WIRE() = default;
    WIRE(uint32_t addr, std::string name, std::string __type)
        : addr(addr), name(name), __type(__type) {}
  };

  class GATE {
  public:
    uint32_t input_cnt, output_cnt;
    std::vector<WIRE> out, in;
    GATE() = default;
    GATE(int in, int out)
        : input_cnt(in), output_cnt(out), out(out), in(in) {}
  };

  std::vector<WIRE> input, output, wire;
  std::map<int, std::string> wire_map, n_map;
  std::vector<std::vector<GATE>> gates;

  void get_input(std::ifstream &in_file, std::stringstream &ss, int fanin, int source_addr)
  {
    // std::cout << "  **get in get_input function**\n";
    std::string line_buf;
    std::getline(in_file, line_buf);
    ss << line_buf;
    int addr_buf;

    while (ss >> addr_buf)
      std::cout << ", " + n_map[addr_buf];

    std::cout << ");\n";
    clear_ss(ss);
  }

  void get_output(std::ifstream &in_file, std::stringstream &ss, int fanout, int source_addr)
  {
    // std::cout << "  **get in get_output function**\n";
    std::string line_buf, name_buf, wire_type, source_buf;
    int int_buf, addr_buf;

    /*
        if (wire_type == "inpt") {
          for (; fanout > 0; --fanout) {
            std::getline(in_file, line_buf);
            ss << line_buf;
            ss >> addr_buf >> name_buf >> wire_type >> source_buf;
            std::cout << "assign gat" + std::to_string(addr_buf) + " = gat" + std::to_string(source_addr) + ";\n";

            n_map.emplace(addr_buf, "gat" + std::to_string(addr_buf));
          }
        }
        else {*/
    for (; fanout > 0; --fanout) {
      std::getline(in_file, line_buf);
      ss << line_buf;
      ss >> addr_buf >> name_buf >> wire_type >> source_buf;
      std::cout << "assign gat" + std::to_string(addr_buf) + " = gat" + std::to_string(source_addr) + ";\n";
      n_map.emplace(addr_buf, "gat" + std::to_string(addr_buf));

      clear_ss(ss);
    }
    //}
  }
}    // namespace detail

void parse_line(std::ifstream &in_file, std::stringstream &ss, std::vector<std::string> &ans)
{
  using namespace detail;
  uint32_t addr_buf, fanin = -1, fanout = -1;
  std::string name_buf, from_gate, wire_type;

  ss >> addr_buf >> name_buf >> wire_type >> fanout >> fanin;
  std::cout << "addr_buf: " << addr_buf << " wire_type: " << wire_type << '\n';
  clear_ss(ss);

  if (fanout == 0) {
    output.emplace_back(addr_buf, name_buf, wire_type);
    n_map.emplace(addr_buf, "gat_out" + std::to_string(addr_buf));
  }

  if (fanin == 0) {
    input.emplace_back(addr_buf, name_buf, wire_type);
    n_map.emplace(addr_buf, "gat" + std::to_string(addr_buf));
  }
  else {
    n_map.emplace(addr_buf, "gat" + std::to_string(addr_buf));
    std::cout << wire_type /*nand*/ << " " << name_buf /*11gat*/ << "(" << n_map[addr_buf] /*gat11*/;
    get_input(in_file, ss, fanin, addr_buf);
  }

  if (fanout > 1)
    get_output(in_file, ss, fanout, addr_buf);
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
  std::vector<std::string> ans;
  std::stringstream ss;
  while (std::getline(in_file, buf)) {
    if (buf[0] != '*') {    // if it's command, read next line.
      std::cout << "parsing line : " << buf << '\n';
      ss << buf;
      parse_line(in_file, ss, ans);
    }
  }
}