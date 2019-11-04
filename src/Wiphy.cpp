#include "Wiphy.h"

namespace wiphynlcontrol {

Wiphy::Wiphy() { setup_maps(); }

Wiphy::Wiphy(const char *name) : name_(name) { setup_maps(); }

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
              {static_cast<void *>(&id_), NL80211_ATTR_WIPHY,
               AttributeValueTypes::UINT32},
              {static_cast<void *>(&name_), NL80211_ATTR_WIPHY_NAME,
               AttributeValueTypes::STRING}},
      },  // Commands::Set
      {
          Commands::Get,
          std::set<AttributeBlock>{
              {static_cast<void *>(&id_), NL80211_ATTR_WIPHY,
               AttributeValueTypes::UINT32},
              {static_cast<void *>(&name_), NL80211_ATTR_WIPHY_NAME,
               AttributeValueTypes::STRING},
              {static_cast<void *>(&frequency_), NL80211_ATTR_WIPHY_FREQ,
               AttributeValueTypes::UINT32}},
      },  // Commands::Get
      {
          Commands::New,
          std::set<AttributeBlock>{
              {static_cast<void *>(&id_), NL80211_ATTR_WIPHY,
               AttributeValueTypes::UINT32},
              {static_cast<void *>(&name_), NL80211_ATTR_WIPHY_NAME,
               AttributeValueTypes::STRING}},
      },  // Commands::New
      {
          Commands::Del,
          std::set<AttributeBlock>{
              {static_cast<void *>(&id_), NL80211_ATTR_WIPHY,
               AttributeValueTypes::UINT32},
              {static_cast<void *>(&name_), NL80211_ATTR_WIPHY_NAME,
               AttributeValueTypes::STRING}},
      },  // Commands::Del
  };      // attribute_map_

  // Identifies the entity.
  identifier_ = {
      .attr_class_member = static_cast<void *>(&name_),
      .attr_type = NL80211_ATTR_WIPHY,
      .attr_val_type = AttributeValueTypes::STRING,
  };
}

Entity::AttributeBlock *Wiphy::get_identifier(void **arg) {
  return &identifier_;
}

}  // namespace wiphynlcontrol