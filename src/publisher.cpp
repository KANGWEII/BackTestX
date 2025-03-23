#include <filesystem>

#include "BackTestX/csv_reader.hpp"

using namespace backtestx;

int main(int argc, char* argv[]) {
  CsvReader test;

  if (argc > 1) {
    auto data = test.ReadCSV(std::filesystem::path(argv[1]));

    for (const auto& row : data) {
      for (const auto& cell : row) {
        std::cout << cell << "\t";
      }
      std::cout << std::endl;
    }
  } else {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
  }

  return 0;
}