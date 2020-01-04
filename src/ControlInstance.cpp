
#include <iostream>
#include <memory>
#include <variant>

#include "netlink/attr.h"

#include "ComControl.h"
#include "Communicator.h"
#include "Interface.h"
#include "Wiphy.h"

namespace wiphynlcontrol {


void link_bss() {
  int index = 3;
  Attribute attr_arg(&index, NL80211_ATTR_IFINDEX, Attribute::ValueTypes::UINT32, NULL);
  const auto arg = std::vector<const Attribute *>{&attr_arg};
  NestedAttr bss_attr_val;
  auto attr_read = Attribute(&bss_attr_val, NL80211_ATTR_BSS, Attribute::ValueTypes::NESTED, NULL);
  const auto read = std::vector<Attribute *>{&attr_read};

  // struct nlattr *bss_attr[NL80211_BSS_MAX + 1] = {};
  // struct nla_policy bss_policy[NL80211_BSS_MAX + 1] = {};

  // bss_policy[NL80211_BSS_TSF] = { type : NLA_U64 };
  // bss_policy[NL80211_BSS_FREQUENCY] = { type : NLA_U32 };
  // bss_policy[NL80211_BSS_BSSID] = { };
  // bss_policy[NL80211_BSS_BEACON_INTERVAL] = { type : NLA_U16 };
  // bss_policy[NL80211_BSS_CAPABILITY] = { type : NLA_U16 };
  // bss_policy[NL80211_BSS_INFORMATION_ELEMENTS] = { };
  // bss_policy[NL80211_BSS_SIGNAL_MBM] = { type : NLA_U32 };
  // bss_policy[NL80211_BSS_SIGNAL_UNSPEC] = { type : NLA_U8 };
  // bss_policy[NL80211_BSS_STATUS] = { type : NLA_U32 };
  // bss_policy[10] = { type : NLA_U32 };

  // bss_attr_val.attr = &bss_attr[0];
  // bss_attr_val.policy = &bss_policy[0];

  ComControl::get_communicator().challenge(NL80211_CMD_TRIGGER_SCAN, Message::Flags::NONE, &arg, &read);

  // Property<NestedAttr> pro(&attr_arg,
  //                           NL80211_ATTR_BSS,
  //                           Attribute::ValueTypes::UINT48,
  //                           NL80211_CMD_GET_SCAN,
  //                           NL80211_CMD_UNSPEC);

}

// void station() {
//     int index = 3;
//     Attribute attr_arg(&index, NL80211_ATTR_IFINDEX, Attribute::ValueTypes::UINT32, NULL);
//     const auto arg = std::vector<const Attribute *>{&attr_arg};
//     // auto read = Attribute(int(), NL80211_ATTR_BSS, Attribute::ValueTypes::STRING);
//     ComControl::get_communicator().challenge(NL80211_CMD_GET_STATION, Message::Flags::DUMP, &arg, NULL);
// }

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
  print(iface.bss_frequency_.value_, "Bss frequency");
}

class ControlInstance {
 public:
  void Main() {
    try {
      // Test
      auto wiphy = std::make_shared<Wiphy>(0);
      auto iface = std::make_shared<Interface>(3);


      // link_bss();
      // iface->get();
      ComControl::get_communicator().trigger_scan(iface->index_.value_);
      iface->bss_frequency_.get();
      print_iface(*iface.get());
      // print_wiphy(*wiphy.get());

      // std::cout << "got name\n";
      // print_wiphy(*wiphy.get());

      // wiphy->get();
      // std::cout << "got wiphy\n";
      // print_wiphy(*wiphy.get());

      // std::cout << "\n\n\n";

      // iface->mac_addr_.get();
      // std::cout << "got mac\n";
      // print_iface(*iface.get());

      // iface->name_.get();
      // std::cout << "got name\n";
      // print_iface(*iface.get());

      // iface->ssid_.get();
      // std::cout << "got ssid\n";
      // print_iface(*iface.get());

      // iface->type_.get();
      // std::cout << "got type\n";
      // print_iface(*iface.get());

      // auto iface2 = std::make_shared<Interface>(3);
      // iface2->get();
      // std::cout << "got iface\n";
      // print_iface(*iface2.get());

      // link_bss();
      // station();

    } catch (std::exception &e) {
      std::cout << "Exception: " << e.what() << "\n";
      std::cout << "Communicator error report: "
                << ComControl::get_global_error_report() << '\n';
    }
  }
};

}  // namespace wiphynlcontrol

int main(int argc, char **argv) {
  using namespace wiphynlcontrol;
  wiphynlcontrol::ControlInstance mynl_base;
  mynl_base.Main();
  return 0;
}

