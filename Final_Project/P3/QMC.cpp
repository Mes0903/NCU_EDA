#include <cmath>
#include <iostream>
#include <vector>

int mani()
{
  std::string line;
  // the example is a + b + c
  int term = 3;    // a, b, c
  std::vector<bool> table;
  std::vector<bool> output(std::pow(2, term));

  table.reserve(term);
  for (int i = 0; i < term; i++)
    table.push_back(false);

  // calculate all cases
  bool flag = false;
  int cur;
  while (!flag) {
    if (!table[term - 1]) {
      table[term - 1] = true;
    }
    else {
      cur = term - 2;
      while (true) {
        if (!table[cur]) {
          table[cur] = true;
          for (int i = cur; i < term; ++i) {
            table[cur] = false;
          }

          break;
        }
        else {
          if (--cur < 0) {
            flag = true;
            break;
          }
        }
      }
    }

    output.push_back(table[0] || table[1] || table[2]);
  }

  // QMC
}