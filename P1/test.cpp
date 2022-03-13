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
    std::string name;

    WIRE() = delete;
    WIRE(uint32_t addr, std::string name)
        : addr(addr), name(name) {}
  };

  std::vector<WIRE> input, output;    // the list of input and output wire
  std::map<uint32_t, std::string> n_map;    // the map of address and wire/gate name
  std::vector<std::string> ans;

  void get_input(std::ifstream &in_file, std::stringstream &ss, std::string &ans_buf)
  {
    std::string line_buf;
    uint32_t addr_buf;
    std::getline(in_file, line_buf);    // read the input line
    ss << line_buf;

    // generate the input line
    while (ss >> addr_buf)
      ans_buf += (", " + n_map[addr_buf]);

    ans_buf += ");\n";
    ans.push_back(std::move(ans_buf));    // push the line into answer body
    clear_ss(ss);
  }

  void get_output(std::ifstream &in_file, std::stringstream &ss, uint32_t fanout, uint32_t source_addr)
  {
    std::string line_buf, name_buf;
    uint32_t addr_buf;

    for (; fanout > 0; --fanout) {
      std::getline(in_file, line_buf);
      ss << line_buf;
      ss >> addr_buf >> name_buf;
      n_map.emplace(addr_buf, "gat" + std::to_string(addr_buf));
      ans.emplace_back("assign " + n_map[addr_buf] + " = " + n_map[source_addr] + ";\n");    // when there are output from somewhere, assign it together.

      clear_ss(ss);
    }
  }
}    // namespace detail

void parse_line(std::ifstream &in_file, std::stringstream &ss)
{
  using namespace detail;
  uint32_t addr_buf, fanin, fanout;
  std::string name_buf, wire_type;

  ss >> addr_buf >> name_buf >> wire_type >> fanout >> fanin;
  clear_ss(ss);

  // if fanout is 0, it's an output line.
  if (fanout == 0) {
    n_map.emplace(addr_buf, "gat_out" + std::to_string(addr_buf));
    output.emplace_back(addr_buf, "gat_out" + std::to_string(addr_buf));
  }

  // if fanin is 0, it's an input line.
  if (fanin == 0) {
    n_map.emplace(addr_buf, "gat" + std::to_string(addr_buf));
    input.emplace_back(addr_buf, "gat" + std::to_string(addr_buf));
  }
  // otherwise, get the input
  else {
    n_map.emplace(addr_buf, "gat" + std::to_string(addr_buf));
    std::string ans_buf = wire_type + " " + name_buf + "(" + n_map[addr_buf];
    get_input(in_file, ss, ans_buf);
  }

  // if the fanout is bigger than 1, get the output.
  if (fanout > 1)
    get_output(in_file, ss, fanout, addr_buf);
}

void print_file(std::ostream &out_file, char *argv)
{
  using namespace detail;

  /* print title */
  std::string file_name = argv;
  file_name = file_name.substr(0, file_name.find(".v"));
  out_file << "`timescale 1ns/1ps\n";
  /*------------*/

  int i_size = input.size(), o_size = output.size();

  /* print module */
  out_file << "module " << file_name << " (";
  for (int i = 0; i < i_size; ++i)
    out_file << input[i].name << ", ";

  for (int i = 0; i < o_size; ++i)
    out_file << output[i].name << (i != o_size - 1 ? ", " : ");\n");
  /*------------*/

  /* print input */
  out_file << "input ";
  for (int i = 0; i < i_size; ++i) {
    out_file << input[i].name << (i != i_size - 1 ? ", " : ";\n");
    n_map.erase(input[i].addr);    // erase the wire in name map, or we will need to check if it had been printed
  }
  /*------------*/

  /* print output */
  out_file << "output ";
  for (int i = 0; i < o_size; ++i) {
    out_file << output[i].name << (i != o_size - 1 ? ", " : ";\n");
    n_map.erase(output[i].addr);    // erase the wire in name map, or we will need to check if it had been printed
  }
  /*------------*/

  /* print wire */
  out_file << "wire ";
  auto n_map_end = n_map.end();
  --n_map_end;
  for (auto it = n_map.begin(); it != n_map.end(); ++it)
    out_file << it->second << (it != n_map_end ? ", " : ";\n");
  /*------------*/

  /* print body */
  for (const auto &str : ans)
    out_file << str;

  /* print endmodule */
  out_file << "endmodule";
  /*------------*/
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
  std::stringstream ss;
  while (std::getline(in_file, buf)) {
    if (buf[0] != '*') {    // if it's command, read next line.
      ss << buf;
      parse_line(in_file, ss);
    }
  }

  /* output ans to the file*/
  std::ofstream out_file(argv[2]);
  if (!out_file.is_open()) {
    std::cerr << "Cannot open file: " << argv[2] << '\n';
    exit(1);
  }

  print_file(out_file, argv[2]);

  return 0;
}