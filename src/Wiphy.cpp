#include "Wiphy.h"

namespace wiphynlcontrol {

Wiphy::Wiphy() { setup_maps(); }  //  TODO

Wiphy::Wiphy(const uint32_t id)
    : id_(std::make_shared<uint32_t>(id)),
      name_(std::make_shared<std::string>()),
      address_(std::make_shared<std::string>()),
      frequency_(std::make_shared<uint32_t>()) {
  setup_maps();
}

void Wiphy::setup_maps() {
  command_map_ = {{Commands::Set, NL80211_CMD_SET_WIPHY},
                  {Commands::Get, NL80211_CMD_GET_WIPHY},
                  {Commands::New, NL80211_CMD_NEW_WIPHY},
                  {Commands::Del, NL80211_CMD_DEL_WIPHY}};

  attribute_map_ = {
      // Here attribute_map_ is initialized.
      // Key - Value mapping is based on Netlink communication knowledge.
      // Useful information can be found in the nl80211.h file.
      //
      // Initialization is done as shown in the schema below:
      //
      // {
      // Key: Commands::Set,
      // Value: std::set<AttributeBlock> {
      // {.....// AttributeBlock..............
      // ......// initialization.........},
      //               .
      //               .
      // {.....// AttributeBlock..............
      // ......// initialization.........}},
      // },
      //
      {
          Commands::Set,
          std::set<AttributeBlock>{
              {id_, NL80211_ATTR_WIPHY, AttributeValueTypes::UINT32},
              {name_, NL80211_ATTR_WIPHY_NAME, AttributeValueTypes::STRING}},
      },  // Commands::Set
      {
          Commands::Get,
          std::set<AttributeBlock>{
              {id_, NL80211_ATTR_WIPHY, AttributeValueTypes::UINT32},
              {name_, NL80211_ATTR_WIPHY_NAME, AttributeValueTypes::STRING},
              {frequency_, NL80211_ATTR_WIPHY_FREQ,
               AttributeValueTypes::UINT32}},
      },  // Commands::Get
      {
          Commands::New,
          std::set<AttributeBlock>{
              {id_, NL80211_ATTR_WIPHY, AttributeValueTypes::UINT32},
              {name_, NL80211_ATTR_WIPHY_NAME, AttributeValueTypes::STRING}},
      },  // Commands::New
      {
          Commands::Del,
          std::set<AttributeBlock>{
              {id_, NL80211_ATTR_WIPHY, AttributeValueTypes::UINT32},
              {name_, NL80211_ATTR_WIPHY_NAME, AttributeValueTypes::STRING}},
      },  // Commands::Del
  };      // attribute_map_

  // Identifies the entity.
  identifier_ = {
      .attr_class_member = id_,
      .attr_type = NL80211_ATTR_WIPHY,
      .attr_val_type = AttributeValueTypes::UINT32,
  };
}

Entity::AttributeBlock *Wiphy::get_identifier(void **arg) {
  return &identifier_;
}

// Gets AttributeBlock for name_ member.
Entity::AttributeBlock *Wiphy::get_name() { return NULL; }

}  // namespace wiphynlcontrol