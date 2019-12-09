#ifndef WIPHYNLCONTROL_INTERFACE_H_
#define WIPHYNLCONTROL_INTERFACE_H_

#define IFACE_ATTR_NUM 5

#include <array>
#include <string>

#include "Entity.h"
#include "Property.h"

namespace wiphynlcontrol {

class Interface : public Entity {
 public:
  explicit Interface(const uint32_t &id);

 public:
  Property<uint32_t> index_;
  Property<std::string> name_;
  Property<uint32_t> type_;
  // mac addr stored as %02x:%02x:%02x:%02x:%02x:%02x string
  Property<std::string> mac_addr_;
  Property<std::string> ssid_;

 private:
  const std::array<struct Attribute *, IFACE_ATTR_NUM> all_attrs;

 public:
  void get();
  // Returns identifier. Overloads abstract method from Entity.
  const uint32_t &get_identifier() const;
  // Overloads abstract method from Entity.
  void set_identifier(const uint32_t &id);
};

}  // namespace wiphynlcontrol
#endif
