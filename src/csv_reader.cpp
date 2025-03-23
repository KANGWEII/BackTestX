#include "BackTestX/csv_reader.hpp"

namespace backtestx {
CsvReader::CsvReader() {}

std::vector<std::vector<std::string>> CsvReader::ReadCSV(
    const std::string& filename) {
  std::vector<std::vector<std::string>> data;
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return data;
  }

  std::string line;
  while (std::getline(file, line)) {
    std::vector<std::string> row;
    std::stringstream ss(line);
    std::string cell;

    while (std::getline(ss, cell, ',')) {
      row.push_back(cell);
    }

    data.push_back(row);
  }

  file.close();
  return data;
}

}  // namespace backtestx