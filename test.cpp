#include <iostream>
#include <memory>

#include "TMonitor.hpp"

int main(int argc, char **argv)
{
  std::string confFile;
  std::unique_ptr<TMonitor> monitor;

  std::cerr << "File check test" << std::endl;
  try {
    confFile = "no_file";
    monitor.reset(new TMonitor(confFile));

  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  std::cerr << "\nJSON check test" << std::endl;
  confFile = "test.json";
  monitor.reset(new TMonitor(confFile));
  monitor->CheckParameters();

  std::cerr << "\nWhen the UPS not working (no response)." << std::endl;
  monitor->FetchData();
  monitor->CheckParameters();

  return 0;
}