// Demonstration of Lib80211Control capabilities.
#ifdef SCAN_DEMONSTRATION
#include <iostream>
#include <memory>

#include "ComControl.h"
#include "Interface.h"

using namespace wiphynlcontrol;

int main () {
  int index = 3;  // index of wlp2s0 interface.
  try {

    // Create Interface object.
    auto iface = std::make_unique<Interface>(index);
    // Trigger scan on wlp2s0.
    ComControl::get_communicator().trigger_scan(iface->index_.value_);
    // Get scan results.
    iface->scan_.get();
    for (auto &i : iface->scan_.value_) {
      std::cout << "freq: " << i.frequency << " | bssid: " << i.bssid << " | ssid: " << i.ssid;
        if (i.status != "")
          std::cout << " | status: " << i.status;
      std::cout << "\n";
    }
  } catch (std::exception &e) {
      std::cout << "Exception: " << e.what() << "\n";
      std::cout << "Communicator error report: " << ComControl::get_global_error_report() << "\n";
  }
  return 0;
}
#endif