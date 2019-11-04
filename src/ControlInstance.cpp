
#include <iostream>
#include "Communicator.h"

using namespace wiphynlcontrol;
using namespace std;

class ControlInstance {
 public:
  void Main() {
    try {
      // Test
      Socket *sock = new Socket();
      Wiphy *wiphy = new Wiphy("phy0");
      Communicator *com = new Communicator();

      com->prepare_message(wiphy, Entity::Commands::Get, NULL);
      com->challenge(sock);

      cout << wiphy->id_ << "\n";
      cout << wiphy->name_ << "\n";
    } catch (exception &e) {
      cout << "Exception: " << e.what() << "\n";
    }
  }
};

int main(int argc, char **argv) {
  ControlInstance mynl_base;
  mynl_base.Main();

  return 0;
}