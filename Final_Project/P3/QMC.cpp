#include <algorithm>
#include <cmath>
#include <iostream>
#include <string_view>
#include <vector>

bool can_merge(const std::string_view a, const std::string_view b)
{
  int cnt = 0, size = a.size();

  for (int i = 0; i < size; ++i) {
    if (a[i] != b[i]) ++cnt;
    if (cnt > 1) return false;
  }

  return true;
}

std::string merge(std::string a, std::string_view b)
{
  int size = a.size();
  for (int i = 0; i < size; ++i) {
    if (a[i] != b[i]) {
      a[i] = '-';
      return a;
    }
  }
}

void Generate_Primed_Implic(std::vector<std::string> &list, std::vector<std::string> &Prime_Implic)
{
  std::vector<bool> flag;
  int size = list.size();
  flag.reserve(size);
  for (int i = 0; i < size; ++i)
    flag.push_back(false);

  std::string s;
  std::vector<std::string> tmp;
  for (int i = 0; i < size; ++i) {
    for (int j = i + 1; j < size; ++j) {
      if (can_merge(list[i], list[j])) {
        s = merge(list[i], list[j]);
        if (std::find(std::begin(tmp), std::end(tmp), s) == std::end(tmp))
          tmp.push_back(std::move(s));

        flag[i] = flag[j] = true;
      }
    }

    if (flag[i] == false)
      Prime_Implic.push_back(list[i]);
  }

  list = std::move(tmp);
}

std::vector<std::string> QMC(std::vector<std::string> &list)
{
  std::vector<std::string> Prime_Implic;
  while (!list.empty())
    Generate_Primed_Implic(list, Prime_Implic);

  return Prime_Implic;
}

int main()
{
  std::string line;
  // the example is a + b + c
  int term = 3;    // a, b, c
  std::string table;
  std::vector<std::string> list;

  table.reserve(term);
  for (int i = 0; i < term; i++)
    table.push_back('0');

  // calculate all cases
  bool flag = false;
  int ans = 0;
  int cur;
  while (!flag) {
    if (table[0] == '1' ||
        table[1] == '1' ||
        table[2] == '1') {
      list.push_back(table);
    }

    if (table[term - 1] == '0') {
      table[term - 1] = '1';
    }
    else {
      cur = term - 2;
      while (true) {
        if (table[cur] == '0') {
          table[cur] = '1';
          for (int i = cur + 1; i < term; ++i)
            table[i] = '0';

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
  }

  std::sort(list.begin(), list.end(),
            [term](const std::string &a, const std::string &b) {
              return a[term] < b[term];
            });

  std::vector<std::string> Prime_Implic = QMC(list);

  for (const auto str : Prime_Implic)
    std::cout << str << '\n';
}