#include "Wiphy.h"

#include <iostream>

namespace wiphynlcontrol {

Wiphy::Wiphy(const uint32_t &id)
    : Entity(id_, NL80211_ATTR_WIPHY, Attribute::ValueTypes::UINT32),
      id_(std::make_shared<uint32_t>(id)),
      name_(std::make_shared<std::string>()),
      address_(std::make_shared<std::string>()),
      frequency_(std::make_shared<uint32_t>()) {}

const std::weak_ptr<Entity::Attribute> Wiphy::get_identifier() const {
  return identifier_;
}

void Wiphy::set_name(const Communicator &com) {
  std::vector<std::unique_ptr<Entity::Attribute>> attr_vec = {
    std::make_unique<Entity::Attribute>(name_, NL80211_ATTR_WIPHY_NAME,
                                        Attribute::ValueTypes::STRING)
  };
}

}  // namespace wiphynlcontrol