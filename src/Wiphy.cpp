#include "Wiphy.h"

#include <iostream>

namespace wiphynlcontrol {

Wiphy::Wiphy(const uint32_t &id)
    : Entity(id_, NL80211_ATTR_WIPHY, Attribute::ValueTypes::UINT32),
      id_(std::make_shared<uint32_t>(id)),
      name_(std::make_shared<std::string>()),
      address_(std::make_shared<std::string>()),
      frequency_(std::make_shared<uint32_t>()) {}

std::weak_ptr<Entity::Attribute> Wiphy::get_identifier() {
  return static_cast<std::weak_ptr<Attribute>>(identifier_);
}

}  // namespace wiphynlcontrol