#ifndef CSV_READER_HPP
#define CSV_READER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace backtestx {
class CsvReader {
 public:
  CsvReader();

  // Do not allow copy
  CsvReader(const CsvReader &) = delete;
  CsvReader &operator=(const CsvReader &) = delete;

  std::vector<std::vector<std::string>> ReadCSV(const std::string &filename);
};
}  // namespace backtestx

#endif /* CSV_READER_HPP */