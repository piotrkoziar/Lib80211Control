
#include <iostream>
#include "Communicator.h"

namespace wiphynlcontrol {

class ControlInstance {
 public:
  void Main() {
    try {
      // Test
      Socket *sock = new Socket();
      Wiphy *wiphy = new Wiphy("phy0");
      Communicator *com = new Communicator(CALLBACK_DEFAULT);

      com->prepare_message(wiphy, Entity::Commands::Get, NULL);
      com->challenge(sock);

      std::cout << wiphy->id_ << "\n";
      std::cout << wiphy->name_ << "\n";
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
