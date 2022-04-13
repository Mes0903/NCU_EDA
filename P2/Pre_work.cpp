#include "Pre_work.h"
#include <stack>
#include <vector>

namespace Schedule_Alg {
  extern std::vector<Type_base::Node> List;    // The List of the nodes.
}

void Pre_work::get_title(std::istream &in_file, int &latency, int &node_num, int &edge_num)
{
  using Schedule_Alg::List;
  using Transform::buf;
  using Transform::ss;
  using Transform::trans_input;
  using Type_base::TYPE;

  std::getline(in_file, buf), trans_input(latency);    // Transform the latency information from source file.
  std::getline(in_file, buf), trans_input(node_num);    // Transform the amount of nodes from source file.
  std::getline(in_file, buf), trans_input(edge_num);    // Transform the amount of edges from source file.
  List.reserve(node_num + 2);    // Reserve the size of the List, two more nodes are added, one for the begin NOP node, one for the end NOP node.
  List.emplace_back(0, TYPE::BEGIN);    // Push the Begin NOP node into the List.
}    // end get_title function

void Pre_work::get_node(std::istream &in_file, const int node_num)
{
  using Schedule_Alg::List;
  using Transform::buf;
  using Transform::ss;
  using Transform::trans_input;
  using namespace Type_base;

  int label;    // The label of node.
  char t;    // The type in the source file, used as the parameter in parser function.
  std::stack<int> output_buf;    // The parent of End NOP node will be pushed into this stack buffer.
  TYPE Type;    // Used to store the type of node.
  for (int i = 0; i < node_num; ++i) {
    std::getline(in_file, buf);    // it will get the label and the corresponding type of the node.
    trans_input(label, t);    // Transform the data into the object
    Type = parse_type(t);    // Parse the type into the enum class type.
    List.emplace_back(label, Type);    // Push it into the node List.

    // If the type of the node is input, the nodes is the child of the Begin NOP node.
    // If the type of the node is output, the nodes is the parent of the End NOP node, store it to the stack buffer.
    if (Type == TYPE::INPUT) {
      List[0].children.push_back(label);
      List[label].parents.push_back(0);
    }
    else if (Type == TYPE::OUTPUT) {
      output_buf.push(label);
    }
  }

  // After all nodes was transformed, build the parent list of the END NOP node.
  List.emplace_back(node_num + 1, TYPE::END);
  while (!output_buf.empty()) {
    label = output_buf.top();
    output_buf.pop();

    List[label].children.push_back(node_num + 1);    // Pusth the END NOP node to the child list of the parent node.
    List[node_num + 1].parents.push_back(label);    // Push the node into the parents list of the END NOP node.
  }
}    // end get_node function

void Pre_work::build_edge(std::istream &in_file, const int edge_num)
{
  using Schedule_Alg::List;
  using Transform::buf;
  using Transform::ss;
  using Transform::trans_input;

  int parent, child;    // The label of parent node and child node.
  for (int i = 0; i < edge_num; ++i) {
    std::getline(in_file, buf);
    trans_input(parent, child);    // Transform the data into the object.

    // Build the edge between two node.
    List[parent].children.push_back(child);
    List[child].parents.push_back(parent);
  }
}    // end build_edge function
