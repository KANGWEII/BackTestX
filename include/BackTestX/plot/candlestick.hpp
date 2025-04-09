#ifndef PLOT_CANDLESTICK_HPP
#define PLOT_CANDLESTICK_HPP

#include <memory>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot/implot.h"
#include "implot/implot_internal.h"
#include <GLFW/glfw3.h>

#include "BackTestX/plot/data_handler.hpp"

namespace backtestx {
namespace plot {

class Candlestick {
 public:
  Candlestick();

  void RenderStockChart(std::shared_ptr<DataHandler>& data_handler_);

 private:
  template <typename T>
  int BinarySearch(const T* arr, int l, int r, T x);
};
}  // namespace plot
}  // namespace backtestx

#endif /* PLOT_CANDLESTICK_HPP */
