# BackTestX

## Description
The repository aims to build a platform for testing trading strategies. The system supports efficient message publishing and subscribing, enabling multiple machines across a network to receive and process real-time data updates. The tool provides an user interface for interacting with the data and visualizes it through detailed charts.
![aapl_candlestick](./docs/Img/demo.gif)

## Installation
Please refer to the [installation guide](/docs/INSTALLATION.md) for detailed instructions on setting up your development environment.

## Running the packages
Please refer to the [running guide](/docs/RUN.md) for detailed instructions on running the sample classification.

## Third-Party Libraries

This project uses the following third-party libraries:

### 1. [Aeron](https://github.com/aeron-io/aeron)
- **License:**  Apache-2.0
- **Description:** Efficient reliable UDP unicast, UDP multicast, and IPC message transport.
- **Why it's used:** For high-performance, low-latency data publishing and subscribing.

### 2. [ImGui](https://github.com/ocornut/imgui)  
- **License:** MIT  
- **Description:** Dear ImGui: Bloat-free Graphical User interface for C++ with minimal dependencies.  
- **Why it’s used:** Used for building the user interface of this project.

### 3. [ImPlot](https://github.com/epezent/implot.git)
- **License:** MIT  
- **Description:** Immediate mode, GPU accelerated plotting library for Dear ImGui.
- **Why it’s used:** Used for rendering the plots of this project.