
#include <iostream>
#include "Communicator.h"

namespace wiphynlcontrol {

class ControlInstance {
 public:
  void Main() {
    try {
      // Test
      Socket *sock = new Socket(CALLBACK_DEBUG);
      Wiphy *wiphy = new Wiphy(0);
      Communicator *com = new Communicator(CALLBACK_DEBUG);

      com->challenge(sock, wiphy, Entity::Commands::Get, NULL);

      std::cout << *wiphy->id_ << "\n";
      std::cout << *wiphy->name_ << "\n";
      std::cout << *wiphy->frequency_ << '\n';
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
