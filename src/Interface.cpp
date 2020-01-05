#include "Interface.h"

#include <vector>

#include "ComControl.h"

namespace wiphynlcontrol {

Interface::Interface(const uint32_t &id)
    : Entity(),
      // Nested Properties (private) - they do not store values.
      // Not implemented any yet.
      // Identifier.
      index_(&get_attribute(index_),
             NL80211_ATTR_IFINDEX,
             Attribute::ValueTypes::UINT32,
             NL80211_CMD_GET_INTERFACE,
             NL80211_CMD_SET_INTERFACE),
      // Properties (public).
      name_(&get_attribute(index_),
            NL80211_ATTR_IFNAME,
            Attribute::ValueTypes::STRING,
            NL80211_CMD_GET_INTERFACE,
            NL80211_CMD_SET_INTERFACE),
      type_(&get_attribute(index_),
            NL80211_ATTR_IFTYPE,
            Attribute::ValueTypes::UINT32,
            NL80211_CMD_GET_INTERFACE,
            NL80211_CMD_SET_INTERFACE),
      mac_addr_(&get_attribute(index_),
                NL80211_ATTR_MAC,
                Attribute::ValueTypes::UINT48,
                NL80211_CMD_GET_INTERFACE,
                NL80211_CMD_SET_INTERFACE),
      ssid_(&get_attribute(index_),
            NL80211_ATTR_SSID,
            Attribute::ValueTypes::STRING,
            NL80211_CMD_GET_INTERFACE,
            NL80211_CMD_SET_INTERFACE),
      scan_ (&get_attribute(index_),
             NL80211_ATTR_BSS,
             Attribute::ValueTypes::SCAN,
             NL80211_CMD_GET_SCAN,
             NL80211_CMD_UNSPEC),
      all_attrs({&get_attribute(index_),
                 &get_attribute(name_),
                 &get_attribute(type_),
                 &get_attribute(mac_addr_),
                 &get_attribute(ssid_)}) {
  set_identifier(id);
}  // namespace wiphynlcontrol

void Interface::get() {
  const auto attr_args = std::vector<const Attribute *>{&get_attribute(index_)};
  const std::vector<Attribute *> attrs(all_attrs.begin(), all_attrs.end());
  ComControl::get_communicator().challenge(
      NL80211_CMD_GET_INTERFACE, Message::Flags::NONE, &attr_args, &attrs);
}

uint32_t Interface::get_identifier() const { return index_.value_; }

void Interface::set_identifier(const uint32_t &id) { index_.value_ = id; }

}  // namespace wiphynlcontrol
