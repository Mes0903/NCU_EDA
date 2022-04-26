#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char *argv[])
{
  std::string InputFilename = "example.v";
  /* open the file and check if it opened successfully */
  std::ifstream in_file(InputFilename);
  if (!in_file.is_open()) {
    std::cerr << "Cannot open file: " << InputFilename << '\n';
    exit(1);
  }
}