#ifndef WIPHYNLCONTROL_WIPHY_H_
#define WIPHYNLCONTROL_WIPHY_H_

#include "Entity.h"

#include <string>

namespace wiphynlcontrol {

class Wiphy : public Entity {
 public:
  Wiphy();
  explicit Wiphy(const uint32_t id);

 public:
  sp_(uint32_t)    id_;
  sp_(std::string) name_;
  sp_(std::string) address_;
  sp_(uint32_t)    frequency_;

 private:
  // Fills command_map_ and attribute_map_.
  // Here apply changes related to command - attribute mapping.
  void setup_maps();

 public:
  // Returns pointer to AttributeBlock with identifier information. TODO
  virtual AttributeBlock *get_identifier(void **arg);
  // Gets AttributeBlock for name_ member.
  AttributeBlock *get_name();
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_WIPHY_H_