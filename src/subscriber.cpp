#include <cstdint>
#include <thread>
#include <csignal>

#include "Aeron.h"
#include "concurrent/SleepingIdleStrategy.h"
#include "FragmentAssembler.h"
#include "util/CommandOptionParser.h"

#include "BackTestX/config/aeron_config.hpp"
#include "BackTestX/graphical/gui.hpp"
#include "BackTestX/plot/data_handler.hpp"

using namespace aeron;
using namespace aeron::util;
using namespace backtestx;

std::atomic<bool> running(true);

void sigIntHandler(int) { running = false; }

static const char opt_help = 'h';
static const char opt_prefix = 'p';
static const char opt_channel = 'c';
static const char opt_stream_id = 's';

static const std::chrono::duration<long, std::milli> IDLE_SLEEP_MS(1);
static const int FRAGMENTS_LIMIT = 10;

struct Settings {
  std::string dir_prefix;
  std::string channel = configuration::DEFAULT_CHANNEL;
  std::int32_t stream_id = configuration::DEFAULT_STREAM_ID;
};

Settings parseCmdLine(CommandOptionParser& cp, int argc, char** argv) {
  cp.parse(argc, argv);
  if (cp.getOption(opt_help).isPresent()) {
    cp.displayOptionsHelp(std::cout);
    exit(0);
  }

  Settings s;

  s.dir_prefix = cp.getOption(opt_prefix).getParam(0, s.dir_prefix);
  s.channel = cp.getOption(opt_channel).getParam(0, s.channel);
  s.stream_id =
      cp.getOption(opt_stream_id).getParamAsInt(0, 1, INT32_MAX, s.stream_id);

  return s;
}

fragment_handler_t DataPlottingHandler(
    std::shared_ptr<backtestx::plot::DataHandler> data_handler) {
  return [data_handler](const AtomicBuffer& buffer, util::index_t offset,
                        util::index_t length, const Header& header) {
    std::string data(reinterpret_cast<const char*>(buffer.buffer()) + offset,
                     static_cast<std::size_t>(length));
    data_handler->ProcessData(data);
  };
}

int main(int argc, char** argv) {
  CommandOptionParser cp;
  cp.addOption(CommandOption(opt_help, 0, 0, "Displays help information."));
  cp.addOption(
      CommandOption(opt_prefix, 1, 1, "Prefix directory for aeron driver."));
  cp.addOption(CommandOption(opt_channel, 1, 1, "Channel."));
  cp.addOption(CommandOption(opt_stream_id, 1, 1, "Stream ID."));

  try {
    Settings settings = parseCmdLine(cp, argc, argv);

    std::cout << "Subscribing to channel " << settings.channel
              << " on Stream ID " << settings.stream_id << std::endl;

    auto data_handler = std::make_shared<backtestx::plot::DataHandler>();

    // Start GUI thread
    graphical::GUI gui;
    gui.SetDataHandler(data_handler);
    gui.StartGUIThread();

    aeron::Context context;

    if (!settings.dir_prefix.empty()) {
      context.aeronDir(settings.dir_prefix);
    }

    context.newSubscriptionHandler([](const std::string& channel,
                                      std::int32_t stream_id,
                                      std::int64_t correlation_id) {
      std::cout << "Subscription: " << channel << " " << correlation_id << ":"
                << stream_id << std::endl;
    });

    context.availableImageHandler([](Image& image) {
      std::cout << "Available image on correlationId=" << image.correlationId()
                << " sessionId=" << image.sessionId();
      std::cout << " at position=" << image.position() << " from "
                << image.sourceIdentity() << std::endl;
    });

    context.unavailableImageHandler([](Image& image) {
      std::cout << "Unavailable image on correlationId="
                << image.correlationId() << " sessionId=" << image.sessionId();
      std::cout << " at position=" << image.position() << " from "
                << image.sourceIdentity() << std::endl;
    });

    std::shared_ptr<Aeron> aeron = Aeron::connect(context);
    signal(SIGINT, sigIntHandler);
    std::int64_t id =
        aeron->addSubscription(settings.channel, settings.stream_id);
    std::shared_ptr<Subscription> subscription = aeron->findSubscription(id);

    while (!subscription) {
      std::this_thread::yield();
      subscription = aeron->findSubscription(id);
    }

    const std::int64_t channel_status = subscription->channelStatus();

    std::cout << "Subscription channel status (id="
              << subscription->channelStatusId() << ") "
              << (channel_status ==
                          ChannelEndpointStatus::CHANNEL_ENDPOINT_ACTIVE
                      ? "ACTIVE"
                      : std::to_string(channel_status))
              << std::endl;

    FragmentAssembler fragment_assembler(DataPlottingHandler(data_handler));
    fragment_handler_t handler = fragment_assembler.handler();
    SleepingIdleStrategy idle_strategy(IDLE_SLEEP_MS);

    while (running) {
      const int fragmentsRead = subscription->poll(handler, FRAGMENTS_LIMIT);
      idle_strategy.idle(fragmentsRead);
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
}