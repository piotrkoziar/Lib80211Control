
#include <iostream>
#include <memory>
#include <variant>

#include "ComControl.h"
#include "Communicator.h"
#include "Interface.h"
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
  // print(*wiphy.bands_, "bands");
  // print(*wiphy.channel_type_, "channel_type");
  // print(*wiphy.txq_params_, "txq_params");
  // print(*wiphy.frequency_, "frequency");
}

static void print_iface(const Interface &iface) {
  print(iface.index_.value_, "index");
  print(iface.name_.value_, "name");
  print(iface.type_.value_, "interface type");
  print(iface.mac_addr_.value_, "mac addr");
  print(iface.ssid_.value_, "SSID");
}

class ControlInstance {
 public:
  void Main() {
    try {
      // Test
      auto wiphy = std::make_shared<Wiphy>(0);
      print_wiphy(*wiphy.get());
      wiphy->name_.get();
      std::cout << "got name\n";
      print_wiphy(*wiphy.get());

      wiphy->get();
      std::cout << "got wiphy\n";
      print_wiphy(*wiphy.get());

      std::cout << "\n\n\n";
      auto iface = std::make_shared<Interface>(4);

      iface->mac_addr_.get();
      std::cout << "got mac\n";
      print_iface(*iface.get());

      iface->name_.get();
      std::cout << "got name\n";
      print_iface(*iface.get());

      iface->ssid_.get();
      std::cout << "got ssid\n";
      print_iface(*iface.get());

      iface->type_.get();
      std::cout << "got type\n";
      print_iface(*iface.get());

      auto iface2 = std::make_shared<Interface>(3);
      iface2->get();
      std::cout << "got iface\n";
      print_iface(*iface2.get());

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
