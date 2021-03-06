#include "Wiphy.h"

#include <vector>

#include "ComControl.h"

namespace wiphynlcontrol {

Wiphy::Wiphy(const uint32_t &id)
    : Entity(),
      // Index is Wiphy's identifier. Must be initialized first.
      index_(&get_attribute(index_),
             NL80211_ATTR_WIPHY,
             Attribute::ValueTypes::UINT32,
             NL80211_CMD_GET_WIPHY,
             NL80211_CMD_SET_WIPHY),
      // Initialization of other Wiphy properties
      name_(&get_attribute(index_),
            NL80211_ATTR_WIPHY_NAME,
            Attribute::ValueTypes::STRING,
            NL80211_CMD_GET_WIPHY,
            NL80211_CMD_SET_WIPHY),
      all_attrs({&get_attribute(index_), &get_attribute(name_)}) {
  set_identifier(id);
}

void Wiphy::get() {
  const auto attr_args = std::vector<const Attribute *>{&get_attribute(index_)};
  const std::vector<Attribute *> attrs(all_attrs.begin(), all_attrs.end());
  ComControl::get_communicator().challenge(
      NL80211_CMD_GET_WIPHY, Message::Flags::NONE, &attr_args, &attrs);
}

uint32_t Wiphy::get_identifier() const { return index_.value_; }

void Wiphy::set_identifier(const uint32_t &id) { index_.value_ = id; }

}  // namespace wiphynlcontrol