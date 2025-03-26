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
#include "util/CommandOptionParser.h"

#include "BackTestX/csv_reader.hpp"
#include "BackTestX/config/configuration.hpp"

using namespace backtestx;
using namespace aeron;
using namespace aeron::util;

std::atomic<bool> running(true);

void SigIntHandler(int signal) { running = false; }

static const char opt_help = 'h';
static const char opt_prefix = 'p';
static const char opt_channel = 'c';
static const char opt_stream_id = 's';
static const char opt_linger = 'l';
static const char opt_file = 'f';

struct Settings {
  std::string dir_prefix;
  std::string channel = configuration::DEFAULT_CHANNEL;
  std::int32_t stream_id = configuration::DEFAULT_STREAM_ID;
  int linger_timeout_ms = configuration::DEFAULT_LINGER_TIMEOUT_MS;
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
  s.linger_timeout_ms =
      cp.getOption(opt_linger)
          .getParamAsInt(0, 0, 60 * 60 * 1000, s.linger_timeout_ms);
  s.file_path = cp.getOption(opt_file).getParam(0, s.file_path);

  return s;
}

int main(int argc, char** argv) {
  CommandOptionParser cp;
  CsvReader csv_reader;
  aeron::Context context;
  std::vector<std::vector<std::string>> data;

  cp.addOption(CommandOption(opt_help, 0, 0, "Displays help information."));
  cp.addOption(
      CommandOption(opt_prefix, 1, 1, "Prefix directory for aeron driver."));
  cp.addOption(CommandOption(opt_channel, 1, 1, "Channel for sending data."));
  cp.addOption(
      CommandOption(opt_stream_id, 1, 1, "Stream ID for sending data."));
  cp.addOption(
      CommandOption(opt_linger, 1, 1, "Linger timeout in milliseconds."));
  cp.addOption(CommandOption(opt_file, 1, 1, "CSV file to read data from."));

  try {
    Settings settings = ParseCmdLine(cp, argc, argv);

    if (!settings.dir_prefix.empty()) {
      context.aeronDir(settings.dir_prefix);
    }

    if (settings.file_path.empty()) {
      throw std::runtime_error("Usage: " + std::string(argv[0]) +
                               " -f <filename>");
    } else {
      data = csv_reader.ReadCSV(std::filesystem::path(settings.file_path));
    }

    std::cout << "Publishing to channel " << settings.channel
              << " on Stream ID " << settings.stream_id << std::endl;

    context.newPublicationHandler(
        [](const std::string& channel, std::int32_t stream_id,
           std::int32_t session_id, std::int64_t correlation_id) {
          std::cout << "Publication: " << channel << " " << correlation_id
                    << ":" << stream_id << ":" << session_id << std::endl;
        });

    std::shared_ptr<Aeron> aeron = Aeron::connect(context);
    signal(SIGINT, SigIntHandler);
    std::int64_t id =
        aeron->addPublication(settings.channel, settings.stream_id);

    std::shared_ptr<Publication> publication = aeron->findPublication(id);

    while (!publication) {
      std::this_thread::yield();
      publication = aeron->findPublication(id);
    }

    const std::int64_t channel_status = publication->channelStatus();

    std::cout << "Publication channel status (id="
              << publication->channelStatusId() << ") "
              << (channel_status ==
                          ChannelEndpointStatus::CHANNEL_ENDPOINT_ACTIVE
                      ? "ACTIVE"
                      : std::to_string(channel_status))
              << std::endl;

    AERON_DECL_ALIGNED(buffer_t buffer, 16);
    concurrent::AtomicBuffer src_buffer(&buffer[0], buffer.size());

    for (const auto& row : data) {
      for (const auto& cell : row) {
        std::cout << cell << "\t";
      }
      std::cout << std::endl;
    }

  } catch (const CommandOptionException& e) {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    cp.displayOptionsHelp(std::cerr);
    return -1;
  } catch (const SourcedException& e) {
    std::cerr << "FAILED: " << e.what() << " : " << e.where() << std::endl;
    return -1;
  } catch (const std::exception& e) {
    std::cerr << "FAILED: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}