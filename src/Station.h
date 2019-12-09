#ifndef WIPHYNLCONTROL_STATION_H_
#define WIPHYNLCONTROL_STATION_H_

#include <array>
#include <string>

#include "Entity.h"
#include "Interface.h"
#include "Property.h"

namespace wiphynlcontrol {

class Station : public Entity {
 public:
  explicit Station(const uint32_t &id);

 public:
  Property<uint32_t> dev_index_;
  Property<std::string> mac_addr_;
  Property<uint32_t> generation_;

 private:
  const std::array<struct Attribute *, 2> all_attrs;

 public:
  void get();
  // Returns identifier. Overloads abstract method from Entity.
  uint32_t get_identifier() const;
  // Overloads abstract method from Entity.
  void set_identifier(const uint32_t &id);
};

}  // namespace wiphynlcontrol
#endif