#ifndef WIPHYNLCONTROL_WIPHY_H_
#define WIPHYNLCONTROL_WIPHY_H_

#define sp_(type) std::shared_ptr<type>

#include "Entity.h"

#include <string>

namespace wiphynlcontrol {

class Wiphy : public Entity {
 public:
  explicit Wiphy(const uint32_t &id);

 public:
  sp_(uint32_t)    id_;
  sp_(std::string) name_;
  sp_(std::string) address_;
  sp_(uint32_t)    frequency_;

 public:
  // Returns std::weak_ptr pointer to Attribute with identifier information.
  std::weak_ptr<Attribute> get_identifier();
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_WIPHY_H_