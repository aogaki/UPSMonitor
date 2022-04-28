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
    std::cerr << e.what() << '\n';
  }

  std::cerr << "JSON check test" << std::endl;
  confFile = "test.json";
  monitor.reset(new TMonitor(confFile));
  monitor->CheckParameters();

  return 0;
}