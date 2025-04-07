#include "BackTestX/graphical/gui.hpp"

namespace backtestx {
namespace graphical {

GUI::GUI() : keep_running_(false), data_handler_(nullptr) {}

GUI::~GUI() {
  keep_running_ = false;
  if (gui_thread_.joinable()) gui_thread_.join();
}

template <typename T>
int GUI::BinarySearch(const T* arr, int l, int r, T x) {
  if (r >= l) {
    int mid = l + (r - l) / 2;
    if (arr[mid] == x) return mid;
    if (arr[mid] > x) return BinarySearch(arr, l, mid - 1, x);
    return BinarySearch(arr, mid + 1, r, x);
  }
  return -1;
}

void GUI::GlfwErrorCallback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void GUI::StartGUIThread() { gui_thread_ = std::thread(&GUI::GUIThread, this); }

void GUI::SetDataHandler(std::shared_ptr<plot::DataHandler> data_handler) {
  data_handler_ = data_handler;
}

void GUI::RenderStockChart() {
  if (!data_handler_) return;

  if (!data_handler_->GetDataReadyFlag()) return;

  std::vector<plot::StockData> stock_data = data_handler_->GetStockData();
  if (stock_data.empty()) return;

  data_handler_->ResetDataReadyFlag();

  const int count = stock_data.size();
  std::vector<double> dates(count);
  std::vector<double> closes(count);
  std::vector<int> volumes(count);
  std::vector<double> opens(count);
  std::vector<double> highs(count);
  std::vector<double> lows(count);

  for (int i = 0; i < count; ++i) {
    dates[i] = stock_data[i].date;
    closes[i] = stock_data[i].close;
    volumes[i] = stock_data[i].volume;
    opens[i] = stock_data[i].open;
    highs[i] = stock_data[i].high;
    lows[i] = stock_data[i].low;
  }

  static ImVec4 bullCol = ImVec4(0.000f, 1.000f, 0.441f, 1.000f);
  static ImVec4 bearCol = ImVec4(0.853f, 0.050f, 0.310f, 1.000f);

  if (ImPlot::BeginPlot("OHLC Chart", ImVec2(-1, 0))) {
    ImPlot::SetupAxes("Date", "Price ($)",
                      ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit,
                      ImPlotAxisFlags_AutoFit);

    ImDrawList* draw_list = ImPlot::GetPlotDrawList();
    double half_width = count > 1 ? (dates[1] - dates[0]) * 0.25f : 0.25f;

    // Tooltip for data information
    if (ImPlot::IsPlotHovered()) {
      ImPlotPoint mouse = ImPlot::GetPlotMousePos();
      mouse.x =
          ImPlot::RoundTime(ImPlotTime::FromDouble(mouse.x), ImPlotTimeUnit_Day)
              .ToDouble();
      float tool_l =
          ImPlot::PlotToPixels(mouse.x - half_width * 1.5, mouse.y).x;
      float tool_r =
          ImPlot::PlotToPixels(mouse.x + half_width * 1.5, mouse.y).x;
      float tool_t = ImPlot::GetPlotPos().y;
      float tool_b = tool_t + ImPlot::GetPlotSize().y;
      ImPlot::PushPlotClipRect();
      draw_list->AddRectFilled(ImVec2(tool_l, tool_t), ImVec2(tool_r, tool_b),
                               IM_COL32(128, 128, 128, 64));
      ImPlot::PopPlotClipRect();
      int idx = BinarySearch(dates.data(), 0, count - 1, mouse.x);
      if (idx != -1) {
        ImGui::BeginTooltip();
        char buf[32];
        ImPlot::FormatDate(ImPlotTime::FromDouble(dates[idx]), buf, 32,
                           ImPlotDateFmt_DayMoYr,
                           ImPlot::GetStyle().UseISO8601);
        ImGui::Text("Date: %s", buf);
        ImGui::Text("Volume: %d", volumes[idx]);
        ImGui::Text("Open: %.2f", opens[idx]);
        ImGui::Text("Close: %.2f", closes[idx]);
        ImGui::Text("High: %.2f", highs[idx]);
        ImGui::Text("Low: %.2f", lows[idx]);
        ImGui::EndTooltip();
      }
    }

    if (ImPlot::BeginItem("AAPL")) {
      ImPlot::GetCurrentItem()->Color = IM_COL32(64, 64, 64, 255);
      if (ImPlot::FitThisFrame()) {
        for (int i = 0; i < count; ++i) {
          ImPlot::FitPoint(ImPlotPoint(dates[i], lows[i]));
          ImPlot::FitPoint(ImPlotPoint(dates[i], highs[i]));
        }
      }

      for (int i = 0; i < count; ++i) {
        ImVec2 open_pos = ImPlot::PlotToPixels(dates[i] - half_width, opens[i]);
        ImVec2 close_pos =
            ImPlot::PlotToPixels(dates[i] + half_width, closes[i]);
        ImVec2 low_pos = ImPlot::PlotToPixels(dates[i], lows[i]);
        ImVec2 high_pos = ImPlot::PlotToPixels(dates[i], highs[i]);
        ImU32 color =
            ImGui::GetColorU32(opens[i] > closes[i] ? bearCol : bullCol);
        draw_list->AddLine(low_pos, high_pos, color);
        draw_list->AddRectFilled(open_pos, close_pos, color);
      }

      ImPlot::EndItem();
    }
    ImPlot::EndPlot();
  }
}

void GUI::GUIThread() {
  keep_running_ = true;

  // Setup window
  glfwSetErrorCallback(GlfwErrorCallback);
  if (!glfwInit()) return;

  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(1280, 720, "BackTestX", NULL, NULL);
  if (window == NULL) return;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Our state
  bool show_imgui_window = false;
  bool show_implot_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  while (keep_running_ && !glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImPlot::ShowDemoWindow();

    // Main window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::Begin("BackTestX", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    // Render the stock chart
    RenderStockChart();

    ImGui::End();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}

}  // namespace graphical
}  // namespace backtestx
