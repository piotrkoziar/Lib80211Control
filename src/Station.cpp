#include "Station.h"

#include "ComControl.h"

namespace wiphynlcontrol {

Station::Station(const uint32_t &id)
    : Entity(),
      dev_index_(&get_attribute(dev_index_),
                 NL80211_ATTR_IFINDEX,
                 Attribute::ValueTypes::UINT32,
                 NL80211_CMD_GET_STATION,
                 NL80211_CMD_UNSPEC),
      mac_addr_(&get_attribute(dev_index_),
                NL80211_ATTR_MAC,
                Attribute::ValueTypes::UINT48,
                NL80211_CMD_GET_STATION,
                NL80211_CMD_UNSPEC),
      generation_(&get_attribute(dev_index_),
                  NL80211_ATTR_GENERATION,
                  Attribute::ValueTypes::UINT32,
                  NL80211_CMD_GET_STATION,
                  NL80211_CMD_UNSPEC),
      all_attrs({&get_attribute(mac_addr_), &get_attribute(generation_)}) {
  set_identifier(id);
}  // namespace wiphynlcontrol

void Station::get() {
  const auto attr_args =
      std::vector<const Attribute *>{&get_attribute(dev_index_)};
  const std::vector<Attribute *> attrs(all_attrs.begin(), all_attrs.end());
  ComControl::get_communicator().challenge(
      NL80211_CMD_GET_STATION, Message::Flags::DUMP, &attr_args, &attrs);
}

uint32_t Station::get_identifier() const {
  return dev_index_.value_;
}

void Station::set_identifier(const uint32_t &id) { dev_index_.value_ = id; }

}  // namespace wiphynlcontrol