#include "Wiphy.h"

#include <iostream>
#include "Attribute.h"

namespace wiphynlcontrol {

Wiphy::Wiphy(const uint32_t &id)
    : Entity(),
      id_(std::make_shared<uint32_t>(id)),
      name_(std::make_shared<std::string>()),
      bands_(std::make_shared<uint32_t>()),
      txq_params_(std::make_shared<uint32_t>()),
      frequency_(std::make_shared<uint32_t>()),
      channel_type_(std::make_shared<uint32_t>()) {}

const std::unique_ptr<Attribute> Wiphy::get_identifier() const {
  return std::move(std::make_unique<Attribute>(id_, NL80211_ATTR_WIPHY,
                                               Attribute::ValueTypes::UINT32));
}

void Wiphy::set_name(Communicator &com) {
  std::vector<std::unique_ptr<Attribute>> attr_vec;

  attr_vec.push_back(std::make_unique<Attribute>(
      name_,
      NL80211_ATTR_WIPHY_NAME,
      Attribute::ValueTypes::STRING));
  attr_vec.push_back(std::make_unique<Attribute>(
      bands_,
      NL80211_ATTR_WIPHY_BANDS,
      Attribute::ValueTypes::UINT32));
  attr_vec.push_back(std::make_unique<Attribute>(
      txq_params_,
      NL80211_ATTR_WIPHY_TXQ_PARAMS,
      Attribute::ValueTypes::UINT32));
  attr_vec.push_back(std::make_unique<Attribute>(
      frequency_,
      NL80211_ATTR_WIPHY_FREQ,
      Attribute::ValueTypes::UINT32));
  attr_vec.push_back(std::make_unique<Attribute>(
      channel_type_,
      NL80211_ATTR_WIPHY_CHANNEL_TYPE,
      Attribute::ValueTypes::UINT32));

  com.challenge(NL80211_CMD_GET_WIPHY, Message::Flags::MATCH, get_identifier(),
                attr_vec);
}

}  // namespace wiphynlcontrol