#include <unistd.h>

#include <memory>

#include "TMonitor.hpp"

int main(int argc, char **argv)
{
  std::string confFile = "/home/aogaki/test/UPSMonitor/settings.json";
  uint32_t checkInterval = 10;

  for (auto i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "-j") {
      confFile = argv[++i];
    } else if (std::string(argv[i]) == "-t") {
      checkInterval = atoi(argv[++i]);
    }
  }

  // Init data handler
  auto monitor = std::make_unique<TMonitor>(confFile);

  while (true) {
    // Fetch data
    monitor->FetchData();

    // Post data
    monitor->PostData();

    sleep(checkInterval);
  }

  return 0;
}