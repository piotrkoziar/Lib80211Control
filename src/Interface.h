#ifndef WIPHYNLCONTROL_INTERFACE_H_
#define WIPHYNLCONTROL_INTERFACE_H_

#include <string>

#include "Entity.h"
#include "Property.h"

namespace wiphynlcontrol {

class Interface : public Entity {
 public:
  explicit Interface(const uint32_t &id);

 public:
  Property<uint32_t> index_;
  Property<std::string> addr_;
  Property<std::string> name_;
  // Property<std::string> ssid_;
  // Property<uint8_t> type_;

 public:
  // Returns identifier. Overloads abstract method from Entity.
  const uint32_t &get_identifier() const;
  // Overloads abstract method from Entity.
  void set_identifier(const uint32_t &id);
};

}  // namespace wiphynlcontrol
#endif