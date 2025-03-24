#include <array>
#include <atomic>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cinttypes>
#include <filesystem>
#include <string>
#include <thread>

#include "Aeron.h"
#include "Configuration.h"
#include "util/CommandOptionParser.h"

#include "BackTestX/csv_reader.hpp"

using namespace backtestx;
using namespace aeron;
using namespace aeron::util;

std::atomic<bool> running(true);

void SigIntHandler(int signal) { running = false; }

static const char opt_help = 'h';
static const char opt_prefix = 'p';
static const char opt_channel = 'c';
static const char opt_stream_id = 's';
static const char opt_messages = 'm';
static const char opt_linger = 'l';
static const char opt_file = 'f';

struct Settings {
  std::string dir_prefix;
  std::string channel = samples::configuration::DEFAULT_CHANNEL;
  std::int32_t stream_id = samples::configuration::DEFAULT_STREAM_ID;
  long long number_of_messages =
      samples::configuration::DEFAULT_NUMBER_OF_MESSAGES;
  int linger_timeout_ms = samples::configuration::DEFAULT_LINGER_TIMEOUT_MS;
  std::string file_path;
};

typedef std::array<std::uint8_t, 256> buffer_t;

Settings ParseCmdLine(CommandOptionParser& cp, int argc, char** argv) {
  cp.parse(argc, argv);
  if (cp.getOption(opt_help).isPresent()) {
    cp.displayOptionsHelp(std::cout);
    exit(EXIT_SUCCESS);
  }

  Settings s;

  s.dir_prefix = cp.getOption(opt_prefix).getParam(0, s.dir_prefix);
  s.channel = cp.getOption(opt_channel).getParam(0, s.channel);
  s.stream_id =
      cp.getOption(opt_stream_id).getParamAsInt(0, 1, INT32_MAX, s.stream_id);
  s.number_of_messages =
      cp.getOption(opt_messages)
          .getParamAsLong(0, 0, INT64_MAX, s.number_of_messages);
  s.linger_timeout_ms =
      cp.getOption(opt_linger)
          .getParamAsInt(0, 0, 60 * 60 * 1000, s.linger_timeout_ms);
  s.file_path = cp.getOption(opt_file).getParam(0, s.file_path);

  return s;
}

int main(int argc, char** argv) {
  CsvReader csv_reader;

  if (argc > 1) {
    auto data = csv_reader.ReadCSV(std::filesystem::path(argv[1]));

    for (const auto& row : data) {
      for (const auto& cell : row) {
        std::cout << cell << "\t";
      }
      std::cout << std::endl;
    }
  } else {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
  }

  return 0;
}