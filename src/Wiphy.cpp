#include "Wiphy.h"

namespace wiphynlcontrol {

Wiphy::Wiphy(const uint32_t &id)
    : Entity(),
      // Index is Wiphy's identifier. Must be initialized first.
      index_(&get_attributes(index_), NL80211_ATTR_WIPHY,
             Attribute::ValueTypes::UINT32, NL80211_CMD_GET_WIPHY,
             NL80211_CMD_SET_WIPHY),
      // Initialization of other Wiphy properties
      name_(&get_attributes(index_), NL80211_ATTR_WIPHY_NAME,
            Attribute::ValueTypes::STRING, NL80211_CMD_GET_WIPHY,
            NL80211_CMD_SET_WIPHY) {
  set_identifier(id);
}

const uint32_t &Wiphy::get_identifier() const { return index_.get_value(); }

void Wiphy::set_identifier(const uint32_t &id) { index_.set_value(id); }

}  // namespace wiphynlcontrol