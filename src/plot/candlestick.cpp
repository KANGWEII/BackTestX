#include "BackTestX/plot/candlestick.hpp"

namespace backtestx {
namespace plot {
Candlestick::Candlestick() {}

template <typename T>
int Candlestick::BinarySearch(const T* arr, int l, int r, T x) {
  if (r >= l) {
    int mid = l + (r - l) / 2;
    if (arr[mid] == x) return mid;
    if (arr[mid] > x) return BinarySearch(arr, l, mid - 1, x);
    return BinarySearch(arr, mid + 1, r, x);
  }
  return -1;
}

void Candlestick::RenderStockChart(
    std::shared_ptr<DataHandler>& data_handler_) {
  if (!data_handler_) {
    std::cerr << "Data handler is not set!" << std::endl;
    return;
  }

  std::vector<StockData> stock_data = data_handler_->GetStockData();
  if (stock_data.empty()) return;

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
    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
    ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.0f");

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

}  // namespace plot
}  // namespace backtestx
