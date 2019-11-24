
#include <iostream>
#include <memory>
#include <variant>
#include "ComControl.h"
#include "Communicator.h"
#include "Wiphy.h"

namespace wiphynlcontrol {

static void print(const std::string &a, const char *args) {
  std::cout << args << " " << a << '\n';
}

static void print(const uint32_t &a, const char *args) {
  std::cout << args << " " << a << '\n';
}

static void print_wiphy(const Wiphy &wiphy) {
  print(wiphy.index_.get_value(), "index");
  print(wiphy.name_.get_value(), "name");
  // print(*wiphy.bands_, "bands");
  // print(*wiphy.channel_type_, "channel_type");
  // print(*wiphy.txq_params_, "txq_params");
  // print(*wiphy.frequency_, "frequency");
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
      wiphy->name_.set("phy00");
      wiphy->name_.get();
      print_wiphy(*wiphy.get());
      wiphy->index_.set_value(0);
      wiphy->name_.set("phy0");

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
