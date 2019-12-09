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
  print(wiphy.index_.value_, "index");
  print(wiphy.name_.value_, "name");
}

static void print_iface(const Interface &iface) {
  print(iface.index_.value_, "index");
  print(iface.name_.value_, "name");
  print(iface.type_.value_, "interface type");
  print(iface.mac_addr_.value_, "mac addr");
  print(iface.ssid_.value_, "SSID");
}

static void print_stat(const Station &stat) {
  print(stat.dev_index_.value_, "dev_index");
  print(stat.mac_addr_.value_, "mac addr");
  print(stat.generation_.value_, "generation");
}

class ControlInstance {
 public:
  void Main() {
    try {
      // Test
      auto wiphy = std::make_shared<Wiphy>(0);
      auto iface = std::make_shared<Interface>(3);

      wiphy->get();
      std::cout << "got wiphy\n";
      print_wiphy(*wiphy.get());

      std::cout << "\n\n\n";

      iface->mac_addr_.get();
      std::cout << "got mac\n";
      iface->name_.get();
      std::cout << "got name\n";
      iface->ssid_.get();
      std::cout << "got ssid\n";
      iface->type_.get();
      std::cout << "got type\n";
      print_iface(*iface.get());

      std::cout << "Trigger scan\n";
      ComControl::get_communicator().trigger_scan(iface->index_.value_);
      iface->scan_.get();
      for (auto &i : iface->scan_.value_) {
        std::cout << "freq: " << i.frequency << " | mac_addr: " << i.mac_address
                  << " | ssid: " << i.ssid;
        if (i.status != "") {
          std::cout << " | status: " << i.status;
        }
        std::cout << "\n";

      }

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

