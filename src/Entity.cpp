#include "Entity.h"

#include <vector>
#include "Property.h"

namespace wiphynlcontrol {

template <typename T>
Attribute &Entity::get_attribute(Property<T> &prop) const {
  return prop.attr_;
}

template Attribute &Entity::get_attribute(Property<uint32_t> &prop) const;
template Attribute &Entity::get_attribute(Property<std::string> &prop) const;
template Attribute &Entity::get_attribute(Property<char> &prop) const;
template Attribute &Entity::get_attribute(
    Property<std::vector<BSSInfo>> &prop) const;
template Attribute &Entity::get_attribute(Property<int8_t> &prop) const;
template Attribute &Entity::get_attribute(Property<uint16_t> &prop) const;
template Attribute &Entity::get_attribute(Property<uint64_t> &prop) const;
template Attribute &Entity::get_attribute(Property<nested_t> &prop) const;
template Attribute &Entity::get_attribute(Property<bool> &prop) const;

}  // namespace wiphynlcontrol
