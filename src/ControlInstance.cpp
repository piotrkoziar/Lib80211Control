
#include <iostream>
#include "Wiphy.h"

namespace wiphynlcontrol {

class ControlInstance {
 public:
  void Main() {
    try {
      // Test
      Socket *sock = new Socket(CALLBACK_DEBUG);
      Wiphy *wiphy = new Wiphy(0);
      Communicator *com = new Communicator(CALLBACK_DEBUG);

      std::cout << *wiphy->id_ << "\n";
      std::cout << *wiphy->name_ << "\n";
      std::cout << *wiphy->frequency_ << '\n';

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
