#include "BackTestX/csv_reader.hpp"

namespace backtestx {
CsvReader::CsvReader() {}

CsvReader::CsvData CsvReader::ReadCSV(const std::string& filename) {
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return data;
  }

  // Read the header line
  std::string line;
  if (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string cell;
    while (std::getline(ss, cell, ',')) {
      data.headers.push_back(cell);
      data.columns[cell] = std::vector<std::string>();
    }
  }

  // Read the data lines
  while (std::getline(file, line)) {
    std::vector<std::string> row;
    std::stringstream ss(line);
    std::string cell;
    int column_index = 0;

    while (std::getline(ss, cell, ',') && column_index < data.headers.size()) {
      row.push_back(cell);
      data.columns[data.headers[column_index]].push_back(cell);
      column_index++;
    }
    data.rows.push_back(row);
  }

  file.close();
  return data;
}
}  // namespace backtestx