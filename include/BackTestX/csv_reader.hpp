#ifndef CSV_READER_HPP
#define CSV_READER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

namespace backtestx {
class CsvReader {
 public:
  CsvReader();

  // Do not allow copy
  CsvReader(const CsvReader &) = delete;
  CsvReader &operator=(const CsvReader &) = delete;

  struct CsvData {
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
    std::unordered_map<std::string, std::vector<std::string>> columns;

    // Access a column by its header
    const std::vector<std::string> &operator[](
        const std::string &header) const {
      return columns.at(header);
    }
  };

  CsvData ReadCSV(const std::string &filename);

 private:
  CsvData data;
};
}  // namespace backtestx

#endif /* CSV_READER_HPP */