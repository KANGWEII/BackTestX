# Running Guide
This guide will walk you through running a sample publish/subscribe for Apple (AAPL) stock data and visualizing it on an Open-High-Close-Low (OHLC) chart.

## Start Aeron Media Driver
The [Aeron Media Driver](https://aeron.io/docs/aeron/media-driver/) manages data transmission (UDP or IPC) for active publications and subscriptions.
```bash
$ cd BackTestX/build/third_party/aeron/binaries
$ ./aeronmd
```

## [Subscriber](../src/subscriber.cpp)
The subscriber is designed to receive data and render the candlestick chart on a separate thread, ensuring that the chart can be updated dynamically as new data arrives. This approach simulates real-time chart plotting, where the chart is continuously refreshed with incoming data, providing a seamless, real-time visual representation of the stock or asset's price movements. By decoupling the data reception from the chart rendering, the system can efficiently handle data updates without blocking the main execution thread. Open a new terminal and start the subscription process.
```bash
$ cd BackTestX/build/bin
$ ./subscriber
```
OR
```bash
# Run the following to display help information
$ ./subscriber -h
```

## [Publisher](../src/publisher.cpp)
 The publisher is responsible for reading data from a CSV file and then publishing it to the subscriber. This approach simulates a dynamic data flow where the publisher acts as the source of information, continuously feeding the system with new data. Open a new terminal and start the publishing process.
```bash
$ cd BackTestX/build/bin
$ ./publisher -f ../../data/AAPL.csv
```
OR
```bash
# Run the following to display help information
$ ./publisher -h
```