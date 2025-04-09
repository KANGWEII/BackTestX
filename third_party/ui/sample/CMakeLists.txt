add_executable(imgui_sample imgui_sample.cpp)
target_link_libraries(imgui_sample PRIVATE ui)

add_executable(implot_sample implot_sample.cpp)
target_link_libraries(implot_sample PRIVATE ui)