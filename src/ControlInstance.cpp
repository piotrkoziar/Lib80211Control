
#include <iostream>
#include <memory>
#include <variant>
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
      Socket *sock = new Socket(CALLBACK_DEBUG);
      auto com = std::make_shared<Communicator>(CALLBACK_DEBUG);
      Wiphy *wiphy = new Wiphy(0);

      print_wiphy(*wiphy);

      wiphy->name_.get();
      wiphy->index_.set_value(12);

      print_wiphy(*wiphy);

      delete (sock);
      delete (wiphy);
    } catch (std::exception &e) {
      std::cout << "Exception: " << e.what() << "\n";
    }
  }
};

}  // namespace wiphynlcontrol

int main(int argc, char **argv) {
  wiphynlcontrol::ControlInstance mynl_base;
  mynl_base.Main();

  return 0;
}
