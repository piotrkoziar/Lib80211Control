
#include <iostream>
#include <variant>
#include "Wiphy.h"

namespace wiphynlcontrol {

static void print(const std::string &a, const char *args) {
  std::cout << args << " " << a << '\n';
}

static void print(const uint32_t &a, const char *args) {
  std::cout << args << " " << a << '\n';
}

static void print_wiphy(const Wiphy &wiphy) {

  print(std::get<std::string>(wiphy.name_.attr_.value), "name");
  // print(*wiphy.id_, "id");
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
      Wiphy *wiphy = new Wiphy(0);
      Communicator *com = new Communicator(CALLBACK_DEBUG);

      print_wiphy(*wiphy);

      wiphy->set_name(*com);

      print_wiphy(*wiphy);

      free(sock);
      free(wiphy);
      free(com);
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
