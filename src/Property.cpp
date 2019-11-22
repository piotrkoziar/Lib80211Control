#include "Property.h"
#include <iostream>
namespace wiphynlcontrol {

Attribute::Attribute(const std::variant<std::string, uint32_t> &val,
                     const Nl80211AttributeTypes &type,
                     const ValueTypes &val_type)
    : value(val), type(type), val_type(val_type) {}

template <typename T>
Property<T>::Property(const Nl80211AttributeTypes &type,
                      const Attribute::ValueTypes &val_type,
                      const Nl80211Commands &cmd)
    : attr_(T(), type, val_type), cmd_(cmd) {}

template <typename T>
const T &Property<T>::get_value() const {
  return std::get<T>(attr_.value);
}

template <typename T>
void Property<T>::set_value(T val) {
  attr_.value = val;
}

template class Property<uint32_t>;
template class Property<std::string>;

}  // namespace wiphynlcontrol