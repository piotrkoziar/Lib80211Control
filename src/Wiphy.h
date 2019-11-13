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
  uint32_t    id_;
  std::string name_;
  std::string address_;
  uint32_t    frequency_;

 private:
  // Fills command_map_ and attribute_map_.
  // Here apply changes related to command - attribute mapping.
  void setup_maps();

 public:
  // Returns pointer to AttributeBlock with identifier information. TODO
  virtual AttributeBlock *get_identifier(void **arg);
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_WIPHY_H_