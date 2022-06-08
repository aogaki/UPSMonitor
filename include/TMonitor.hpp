#ifndef TMonitor_hpp
#define TMonitor_hpp 1

#include <curl/curl.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Like a all in one class...

class UPSInfo
{
 public:
  int remainingTime;
  bool isHealthy;
  uint64_t timeStamp;
  std::string serverName;
  std::string address;

  UPSInfo()
  {
    remainingTime = 0;
    isHealthy = false;
    timeStamp = 0;
    serverName = "";
    address = "";
  };
  ~UPSInfo(){};

  void Dump()
  {
    std::cout << "Server name: " << serverName << "\n"
              << "IP address: " << address << "\n"
              << "Remaining time: " << remainingTime << "\n"
              << "Is Healthy: " << isHealthy << "\n"
              << "Timestamp: " << timeStamp << "\n"
              << std::endl;
  };
};

class DBInfo
{
 public:
  std::string address;
  int port;

  DBInfo()
  {
    address = "";
    port = 0;
  };
  ~DBInfo(){};

  void Dump()
  {
    std::cout << "IP address: " << address << "\n"
              << "Port: " << port << "\n"
              << std::endl;
  };
};

class TMonitor
{
 public:
  TMonitor();
  explicit TMonitor(std::string json);
  ~TMonitor();

  void LoadParameters(std::string json) { ParseParameters(json); };
  void CheckParameters();

  void FetchData();
  void PostData();

  void Test();

 private:
  void ParseParameters(std::string parFileName);
  std::vector<UPSInfo> fVecUPS;
  std::vector<DBInfo> fVecDB;

  // For CURL
  static size_t CallbackFnc(char *ptr, size_t size, size_t nmemb,
                            std::string *stream);
  typedef void (*deleter)(CURL *);
  std::unique_ptr<CURL, deleter> fCurlHandler;
  bool CheckCurlHandler();
};

#endif