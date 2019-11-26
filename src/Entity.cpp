#include "Entity.h"
#include "Property.h"

namespace wiphynlcontrol {

template <typename T>
const Attribute &Entity::get_attributes(const Property<T> &prop) const {
  return prop.attr_;
}

template const Attribute &Entity::get_attributes(
    const Property<uint32_t> &prop) const;
template const Attribute &Entity::get_attributes(
    const Property<std::string> &prop) const;

}  // namespace wiphynlcontrol
