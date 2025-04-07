#ifndef GRAPHICAL_GUI_HPP
#define GRAPHICAL_GUI_HPP

#include <atomic>
#include <stdio.h>
#include <thread>
#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot/implot.h"
#include "implot/implot_internal.h"
#include <GLFW/glfw3.h>

#include "BackTestX/plot/data_handler.hpp"

namespace backtestx {
namespace graphical {
class GUI {
 public:
  GUI();
  ~GUI();

  // Do not allow copy
  GUI(const GUI&) = delete;
  GUI& operator=(const GUI&) = delete;

  template <typename T>
  int BinarySearch(const T* arr, int l, int r, T x);

  static void GlfwErrorCallback(int error, const char* description);

  void StartGUIThread();

  void SetDataHandler(std::shared_ptr<plot::DataHandler> data_handler);

 private:
  std::atomic_bool keep_running_;
  std::thread gui_thread_;

  std::shared_ptr<plot::DataHandler> data_handler_;

  void GUIThread();

  void RenderStockChart();
};
}  // namespace graphical
}  // namespace backtestx

#endif /* GRAPHICAL_GUI_HPP */
