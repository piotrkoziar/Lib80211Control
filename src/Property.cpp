#include "Property.h"

#include <string>
#include "ComControl.h"
#include "Message.h"

namespace wiphynlcontrol {

template <typename T>
Property<T>::Property(const Attribute &owner_id,
                      const Nl80211AttributeTypes &type,
                      const Attribute::ValueTypes &val_type,
                      const Nl80211Commands &cmd)
    : owner_identifier_(owner_id), attr_(T(), type, val_type), cmd_(cmd) {}

template <typename T>
const T &Property<T>::get_value() const {
  return std::get<T>(attr_.value);
}

template <typename T>
void Property<T>::set_value(T val) {
  attr_.value = val;
}

template <typename T>
const T &Property<T>::get() {
  std::vector<Attribute *> attr_vec = std::vector<Attribute *>{&attr_};
  ComControl::get_communicator().challenge(cmd_, Message::Flags::MATCH,
                                           owner_identifier_,
                                           attr_vec);
  return std::get<T>(attr_.value);
}

template class Property<uint32_t>;
template class Property<std::string>;

}  // namespace wiphynlcontrol