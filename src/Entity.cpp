#include "Entity.h"

#include "Property.h"

namespace wiphynlcontrol {

template <typename T>
Attribute &Entity::get_attribute(Property<T> &prop) const {
  return prop.attr_;
}

template Attribute &Entity::get_attribute(Property<uint8_t> &prop) const;
template Attribute &Entity::get_attribute(Property<uint32_t> &prop) const;
template Attribute &Entity::get_attribute(Property<std::string> &prop) const;

}  // namespace wiphynlcontrol
