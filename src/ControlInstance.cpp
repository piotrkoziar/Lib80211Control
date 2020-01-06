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
  print(stat.generation_.value_, "generation");
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

      sta->generation_.get();
      std::cout << "got generation\n";
      sta->mac_addr_.get();
      std::cout << "got sta mac addr\n";

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

      print_wiphy(*wiphy.get());
      print_iface(*iface.get());
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

