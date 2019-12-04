#include "Interface.h"

namespace wiphynlcontrol {

Interface::Interface(const uint32_t &id)
    : Entity(),
      index_(&get_attributes(index_), NL80211_ATTR_IFINDEX,
             Attribute::ValueTypes::UINT32, NL80211_CMD_GET_INTERFACE,
             NL80211_CMD_SET_INTERFACE),
      name_(&get_attributes(index_), NL80211_ATTR_IFNAME,
            Attribute::ValueTypes::STRING, NL80211_CMD_GET_INTERFACE,
            NL80211_CMD_SET_INTERFACE),
      type_(&get_attributes(index_), NL80211_ATTR_IFTYPE,
            Attribute::ValueTypes::UINT32, NL80211_CMD_GET_INTERFACE,
            NL80211_CMD_SET_INTERFACE),
      mac_addr_(&get_attributes(index_), NL80211_ATTR_MAC,
                Attribute::ValueTypes::UINT48, NL80211_CMD_GET_INTERFACE,
                NL80211_CMD_SET_INTERFACE),
      ssid_(&get_attributes(index_), NL80211_ATTR_SSID,
            Attribute::ValueTypes::STRING, NL80211_CMD_GET_INTERFACE,
            NL80211_CMD_SET_INTERFACE) {
  set_identifier(id);
}

const uint32_t &Interface::get_identifier() const { return index_.get_value(); }

void Interface::set_identifier(const uint32_t &id) { index_.set_value(id); }

}  // namespace wiphynlcontrol