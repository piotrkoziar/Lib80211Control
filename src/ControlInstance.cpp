// Demonstration of Lib80211Control capabilities.

#include <iostream>
#include <memory>

#include "ComControl.h"
#include "Interface.h"
#include "Station.h"
#include "Wiphy.h"

namespace wiphynlcontrol {

static void print(const std::string &a, const char *args) {
  std::cout << args << ": " << a << '\n';
}
static void print(const uint32_t &a, const char *args) {
  std::cout << args << ": " << a << '\n';
}
static void print(const bool &a, const char *args) {
std::string val = (a) ? "true" : "false";
  std::cout << args << ": " << val << '\n';
}
static void print(const int8_t &a, const char *args) {
  // std::cout << args << ": " << a << '\n';
  	printf("%s: %d\n", args, a);
}
static void print(const uint16_t &a, const char *args) {
  std::cout << args << ": " << a << '\n';
}
static void print(const uint64_t &a, const char *args) {
  std::cout << args << ": " << a << '\n';
}

static void print_wiphy(const Wiphy &wiphy) {
  std::cout << "Wiphy:\n";
  print(wiphy.index_.value_, "index");
  print(wiphy.name_.value_, "name");
}

static void print_iface(const Interface &iface) {
  std::cout << "Interface:\n";
  print(iface.index_.value_, "index");
  print(iface.name_.value_, "name");
  print(iface.type_.value_, "interface type");
  print(iface.mac_addr_.value_, "mac addr");
  print(iface.ssid_.value_, "SSID");
}

static void print_stat(const Station &stat) {
  std::cout << "Station:\n";
  print(stat.dev_index_.value_, "dev_index");
  print(stat.mac_addr_.value_, "mac addr");
  print(stat.inactive_time_.value_, "inactive time [ms]");

  print(stat.rx_bytes_.value_, "total received bytes");
  print(stat.tx_bytes_.value_, "total transmitted bytes");
  print(stat.rx_packets_.value_, "total packets received");

  print(stat.tx_packets_.value_, "total packets transmitted");
  print(stat.beacon_rx_.value_, "beacon received");
  print(stat.info_signal_.value_, "info signal [dBm]");

  print(stat.info_offset_.value_, "info offset [us]");
  print(stat.tx_retries_.value_, "tx retries");
  print(stat.tx_failed_.value_, "tx failed");

  print(stat.beacon_loss_.value_, "beacon loss");
  print(stat.tx_bitrate_.value_ / 10, "tx bitrate [MBit/s]");
  print(stat.rx_bitrate_.value_ / 10, "rx bitrate [MBit/s]");

  print(stat.short_preamble_.value_, "short preamble");
  print(stat.short_slot_time_.value_, "short slot time");
  print(stat.beacon_interval_.value_, "beacon interval");
}

class ControlInstance {
 public:
  void Main() {
    try {
      // Test
      auto wiphy = std::make_unique<Wiphy>(0);
      auto iface = std::make_unique<Interface>(3);
      auto sta   = std::make_unique<Station>(iface->get_identifier());

      wiphy->get();
      std::cout << "got wiphy\n";

      std::cout << "\n\n\n";

      iface->mac_addr_.get();
      std::cout << "got mac\n";
      iface->name_.get();
      std::cout << "got name\n";
      iface->ssid_.get();
      std::cout << "got ssid\n";
      iface->type_.get();
      std::cout << "got type\n";

      std::cout << "\n\n\n";

      std::cout << "Trigger scan\n";
      ComControl::get_communicator().trigger_scan(iface->index_.value_);
      iface->scan_.get();
      for (auto &i : iface->scan_.value_) {
        std::cout << "freq: " << i.frequency << " | bssid: " << i.bssid
                  << " | ssid: " << i.ssid;
        if (i.status != "") {
          std::cout << " | status: " << i.status;
        }
        std::cout << "\n";
      }

      sta->mac_addr_.get();
      std::cout << "got sta mac addr\n";
      sta->inactive_time_.get();
      std::cout << "got inactive_time\n";
      sta->rx_bytes_.get();
      std::cout << "got rx_bytes_ \n";
      sta->tx_bytes_.get();
      std::cout << "got tx_bytes_ \n";
      sta->rx_packets_.get();
      std::cout << "got rx_packets_ \n";
      sta->tx_packets_.get();
      std::cout << "got tx_packets_ \n";
      sta->beacon_rx_.get();
      std::cout << "got beacon_rx_ \n";
      sta->info_signal_.get();
      std::cout << "got info_signal_ \n";
      sta->info_offset_.get();
      std::cout << "got info_offset_ \n";
      sta->tx_retries_.get();
      std::cout << "got tx_retries_ \n";
      sta->tx_failed_.get();
      std::cout << "got tx_failed_ \n";
      sta->beacon_loss_.get();
      std::cout << "got beacon_loss_ \n";
      sta->tx_bitrate_.get();
      std::cout << "got tx_bitrate_ \n";
      sta->rx_bitrate_.get();
      std::cout << "got rx_bitrate_ \n";
      sta->short_preamble_.get();
      std::cout << "got short_preamble_\n";
      sta->short_slot_time_.get();
      std::cout << "got short_slot_time_\n";
      sta->beacon_interval_.get();
      std::cout << "got beacon_interval_\n";

      std::cout << "\n\n\n";

      print_wiphy(*wiphy.get());
      std::cout << "\n\n\n";
      print_iface(*iface.get());
      std::cout << "\n\n\n";
      print_stat(*sta.get());

    } catch (std::exception &e) {
      std::cout << "Exception: " << e.what() << "\n";
      std::cout << "Communicator error report: "
                << ComControl::get_global_error_report() << '\n';
    }
  }
};

}  // namespace wiphynlcontrol

int main(int argc, char **argv) {
  wiphynlcontrol::ControlInstance mynl_base;
  mynl_base.Main();
  return 0;
}

