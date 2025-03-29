#ifndef CONFIG_CONFIGURATION_HPP
#define CONFIG_CONFIGURATION_HPP

#include <string>
#include <cstdint>

namespace backtestx {
namespace configuration {

const static std::string DEFAULT_CHANNEL = "aeron:udp?endpoint=localhost:20121";
const static std::int32_t DEFAULT_STREAM_ID = 1001;
const static int DEFAULT_LINGER_TIMEOUT_MS = 0;

}  // namespace configuration
}  // namespace backtestx

#endif /* CONFIG_CONFIGURATION_HPP */
