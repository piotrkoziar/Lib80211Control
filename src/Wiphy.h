#ifndef WIPHYNLCONTROL_WIPHY_H_
#define WIPHYNLCONTROL_WIPHY_H_

#include <array>
#include <string>

#include "Entity.h"
#include "Property.h"

#define WIPHY_ATTR_NUM 2

namespace wiphynlcontrol {

class Wiphy : public Entity {
 public:
  explicit Wiphy(const uint32_t &id);

 public:
  Property<uint32_t> index_;
  Property<std::string> name_;

 private:
  const std::array<struct Attribute *, WIPHY_ATTR_NUM> all_attrs;

 public:
  void get();
  // Returns identifier. Overloads abstract method from Entity.
  uint32_t get_identifier() const;
  // Overloads abstract method from Entity.
  void set_identifier(const uint32_t &id);
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_WIPHY_H_