
#include "TMonitor.hpp"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "influxdb.hpp"

TMonitor::TMonitor() : fCurlHandler(curl_easy_init(), &curl_easy_cleanup)
{
  CheckCurlHandler();
}
TMonitor::TMonitor(std::string json)
    : fCurlHandler(curl_easy_init(), &curl_easy_cleanup)
{
  CheckCurlHandler();
  ParseParameters(json);
}

TMonitor::~TMonitor() {}

bool TMonitor::CheckCurlHandler()
{
  if (!fCurlHandler) {
    std::cerr << "CURL initialization was failed." << std::endl;
    return false;
  } else {
    return true;
  }
}

size_t TMonitor::CallbackFnc(char *ptr, size_t size, size_t nmemb,
                             std::string *stream)
{
  int dataLength = size * nmemb;
  // if (ptr != nullptr) stream->append(ptr, dataLength);
  if (ptr != nullptr) stream->assign(ptr, dataLength);
  return dataLength;
}

void TMonitor::ParseParameters(std::string parFileName)
{
  std::ifstream input(parFileName);
  if (!input.is_open()) {
    std::cerr << "Config file: " << parFileName << " not found." << std::endl;
    throw std::invalid_argument("File not found");
  } else {
    nlohmann::json json;
    input >> json;
    input.close();

    auto ups = json["UPS"];
    auto db = json["DB"];

    const auto nUPSs = ups.size();
    for (auto iUPS = 0; iUPS < nUPSs; iUPS++) {
      UPSInfo upsInfo;
      upsInfo.address = ups[iUPS]["address"];
      upsInfo.serverName = ups[iUPS]["name"];
      fVecUPS.push_back(upsInfo);
    }

    const auto nDBs = db.size();
    for (auto iDB = 0; iDB < nDBs; iDB++) {
      DBInfo dbInfo;
      dbInfo.address = db[iDB]["address"];
      dbInfo.port = db[iDB]["port"];
      fVecDB.push_back(dbInfo);
    }
  }
}

void TMonitor::CheckParameters()
{
  std::cout << "\nUPS" << std::endl;
  for (auto &&ups : fVecUPS) {
    ups.Dump();
  }

  std::cout << "\nDB" << std::endl;
  for (auto &&db : fVecDB) {
    db.Dump();
  }
}

void TMonitor::FetchData()
{
  std::string dumpState;
  curl_easy_setopt(fCurlHandler.get(), CURLOPT_WRITEDATA, &dumpState);
  curl_easy_setopt(fCurlHandler.get(), CURLOPT_WRITEFUNCTION, CallbackFnc);
  for (auto &&ups : fVecUPS) {
    auto cgi = ups.address + "/cgi-bin/realInfo.cgi";
    curl_easy_setopt(fCurlHandler.get(), CURLOPT_URL, cgi.c_str());

    ups.timeStamp = time(nullptr) * 1000000000;

    auto errCode = curl_easy_perform(fCurlHandler.get());
    if (errCode == CURLE_OK) {
      std::string dbStatus;
      std::stringstream ss = std::stringstream(dumpState);
      auto counter = 0;
      while (std::getline(ss, dbStatus) && (counter < 10)) {
        if (dbStatus != "") {
          if (counter == 0) {  // UPS mode.  Make enum, lazy bum!!
            if (dbStatus == "Line Mode")
              ups.isHealthy = true;
            else
              ups.isHealthy = false;
          }

          if (counter == 9) {  // Battery remaining time
            ups.remainingTime = stoi(dbStatus);
          }

          counter++;
        }
      }
    } else {  // something trouble
      ups.isHealthy = false;
      ups.remainingTime = 0;
    }
  }
}

void TMonitor::PostData()
{
  for (auto &&db : fVecDB) {
    auto server = influxdb_cpp::server_info(db.address, db.port, "Annealing");
    std::string resp = "";
    for (auto &&ups : fVecUPS) {
      auto isHealthy = 0;
      if (ups.isHealthy) isHealthy = 1;

      try {
        influxdb_cpp::builder()
            .meas("UPS_Monitor")
            .tag("name", ups.serverName)
            .field("lineMode", isHealthy)
            .field("remaining_time", ups.remainingTime)
            .timestamp(ups.timeStamp)
            .post_http(server, &resp);
      } catch (const std::exception &e) {
        std::cerr << "InfluxDB: " << e.what() << "\n" << resp << std::endl;
      }
    }
  }
}