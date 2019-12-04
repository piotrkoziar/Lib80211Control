
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
  print(wiphy.index_.get_value(), "index");
  print(wiphy.name_.get_value(), "name");
  // print(*wiphy.bands_, "bands");
  // print(*wiphy.channel_type_, "channel_type");
  // print(*wiphy.txq_params_, "txq_params");
  // print(*wiphy.frequency_, "frequency");
}

static void print_iface(const Interface &iface) {
  print(iface.index_.get_value(), "index");
  print(iface.name_.get_value(), "name");
  print(iface.type_.get_value(), "interface type");

  printf("%s: %02x:%02x:%02x:%02x:%02x:%02x\n", "mac addr",
         iface.mac_addr_.get_value().c_str()[0] & 0xff,
         iface.mac_addr_.get_value().c_str()[1] & 0xff,
         iface.mac_addr_.get_value().c_str()[2] & 0xff,
         iface.mac_addr_.get_value().c_str()[3] & 0xff,
         iface.mac_addr_.get_value().c_str()[4] & 0xff,
         iface.mac_addr_.get_value().c_str()[5] & 0xff);

  print(iface.ssid_.get_value(), "SSID");
}

class ControlInstance {
 public:
  void Main() {
    try {
      // Test
      auto wiphy = std::make_shared<Wiphy>(0);
      print_wiphy(*wiphy.get());
      wiphy->name_.get();
      print_wiphy(*wiphy.get());
      // wiphy->name_.set("myphy00");
      // wiphy->name_.get();
      // print_wiphy(*wiphy.get());
      // wiphy->name_.set("phy0");
      // wiphy->index_.set_value(0);
      // wiphy->name_.set("phy0");
      // wiphy->name_.get();
      std::cout << "\n\n\n";
      auto iface = std::make_shared<Interface>(3);
      // auto iface2 = std::make_shared<Interface>(4);
      // iface->index_.set();
      // print_iface(*iface.get());
      // iface->index_.get();
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
      // iface2->addr_.get();
      // iface2->name_.get();
      // iface->addr_.get();
      // print_iface(*iface2.get());

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
