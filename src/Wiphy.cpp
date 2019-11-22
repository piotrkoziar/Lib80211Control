#include "Wiphy.h"

#include <iostream>

namespace wiphynlcontrol {

Wiphy::Wiphy(const uint32_t &id)
    : Entity(),
      index_(NL80211_ATTR_WIPHY, Attribute::ValueTypes::UINT32,
             NL80211_CMD_GET_WIPHY),
      name_(NL80211_ATTR_WIPHY_NAME, Attribute::ValueTypes::STRING,
            NL80211_CMD_GET_WIPHY) {}

const uint32_t &Wiphy::get_identifier() const {
  return index_.get();
}

void Wiphy::set_identifier(const uint32_t &id) {
  index_.set(id);
}

}  // namespace wiphynlcontrol