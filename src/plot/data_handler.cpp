#include "BackTestX/plot/data_handler.hpp"

#include <sstream>
#include <algorithm>

namespace backtestx {
namespace plot {
DataHandler::DataHandler() {}
DataHandler::~DataHandler() {}

void DataHandler::ProcessData(const std::string& data) {
  std::lock_guard<std::mutex> lock(data_mutex_);

  std::stringstream ss(data);
  std::string date, volume, close, open, high, low;

  if (std::getline(ss, date, ',') && std::getline(ss, close, ',') &&
      std::getline(ss, volume, ',') && std::getline(ss, open, ',') &&
      std::getline(ss, high, ',') && std::getline(ss, low)) {
    auto remove_dollar = [](std::string& str) {
      str.erase(std::remove(str.begin(), str.end(), '$'), str.end());
    };

    remove_dollar(close);
    remove_dollar(open);
    remove_dollar(high);
    remove_dollar(low);

    try {
      StockData stock_data;
      stock_data.date = std::stod(date);
      stock_data.close = std::stod(close);
      stock_data.volume = std::stod(volume);
      stock_data.open = std::stod(open);
      stock_data.high = std::stod(high);
      stock_data.low = std::stod(low);

      dates_.push_back(stock_data.date);
      closes_.push_back(stock_data.close);
      volumes_.push_back(stock_data.volume);
      opens_.push_back(stock_data.open);
      highs_.push_back(stock_data.high);
      lows_.push_back(stock_data.low);

      data_ready_ = true;
    } catch (const std::exception& e) {
      std::cerr << "Error processing data: " << e.what() << std::endl;
    }
  } else {
    std::cerr << "Error parsing data: " << data << std::endl;
  }
}

bool DataHandler::GetDataReadyFlag() const { return data_ready_.load(); }

void DataHandler::ResetDataReadyFlag() { data_ready_ = false; }

std::vector<StockData> DataHandler::GetStockData() {
  std::lock_guard<std::mutex> lock(data_mutex_);

  std::vector<StockData> result;
  size_t size = dates_.size();

  for (size_t i = 0; i < size; ++i) {
    StockData data;
    data.date = dates_[i];
    data.close = closes_[i];
    data.volume = volumes_[i];
    data.open = opens_[i];
    data.high = highs_[i];
    data.low = lows_[i];
    result.push_back(data);
  }

  return result;
}

}  // namespace plot
}  // namespace backtestx