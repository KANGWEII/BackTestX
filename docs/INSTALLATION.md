# Installation Guide

This code should compile on any recent Linux distribution with a C++11/14/17-compatible compiler. Development and testing were performed on Ubuntu 24.04.2 LTS.

## Install Dependencies
```bash
$ sudo apt-get install -y build-essential cmake libglfw3-dev
```

## Getting this repository
```bash
$ git clone --recursive https://github.com/KANGWEII/BackTestX.git
```

## Build the Workspace
After installing the dependencies, you can build the workspace by running the following commands:

You can customize the build using the following flags:

| Option                | Default | Description                                 |
|-----------------------|---------|---------------------------------------------|
| `IMGUI_IMPLOT_SAMPLE` | OFF     | Option to build ImGui and ImPlot samples    |
| `ENABLE_LOGGING`      | ON      | Enable logging module                       |

```bash
$ cd BackTestX
$ mkdir build && cd build
$ cmake ..
$ make -j
```