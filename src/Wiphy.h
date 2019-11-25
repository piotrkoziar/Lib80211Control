#ifndef WIPHYNLCONTROL_WIPHY_H_
#define WIPHYNLCONTROL_WIPHY_H_

#include <string>

#include "Entity.h"
#include "Property.h"

namespace wiphynlcontrol {

class Wiphy : public Entity {
 public:
  explicit Wiphy(const uint32_t &id);

 public:
  Property<uint32_t> index_;
  Property<std::string> name_;

 public:
  // Returns identifier. Overloads abstract method from Entity.
  const uint32_t &get_identifier() const;
  // Overloads abstract method from Entity.
  void set_identifier(const uint32_t &id);
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_WIPHY_H_