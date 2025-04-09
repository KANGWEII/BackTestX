#ifndef PLOT_DATA_HANDLER_HPP
#define PLOT_DATA_HANDLER_HPP

#include <string>
#include <vector>
#include <mutex>
#include <deque>
#include <atomic>
#include <iostream>

namespace backtestx {
namespace plot {

struct StockData {
  double date;
  double close;
  int volume;
  double open;
  double high;
  double low;
};

class DataHandler {
 public:
  DataHandler();
  ~DataHandler();

  void ProcessData(const std::string& data);

  bool GetDataReadyFlag() const;
  void ResetDataReadyFlag();
  std::vector<StockData> GetStockData();

 private:
  std::mutex data_mutex_;
  std::atomic<bool> data_ready_;

  // Storage for financial data
  std::vector<double> dates_;
  std::vector<double> closes_;
  std::vector<int> volumes_;
  std::vector<double> opens_;
  std::vector<double> highs_;
  std::vector<double> lows_;
};
}  // namespace plot
}  // namespace backtestx

#endif /* PLOT_DATA_HANDLER_HPP */
