#include "Property.h"

#include <string>
#include "ComControl.h"
#include "Exception.h"
#include "Message.h"

namespace wiphynlcontrol {

template <typename T>
Property<T>::Property(const Attribute &owner_id,
                      const Nl80211AttributeTypes &type,
                      const Attribute::ValueTypes &value_type,
                      const Nl80211Commands &cmd_get,
                      const Nl80211Commands &cmd_set)
    : owner_identifier_(owner_id),
      attr_(T(), type, value_type),
      cmd_get_(cmd_get),
      cmd_set_(cmd_set) {}

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
  auto attr_args = std::vector<Attribute *>{&owner_identifier_};
  auto attr_read = std::vector<Attribute *>{&attr_};
  ComControl::get_communicator().challenge(cmd_get_, Message::Flags::MATCH,
                                           &attr_args, &attr_read);
  return std::get<T>(attr_.value);
}

template <typename T>
void Property<T>::set(const T &arg) {
  // Create attribute object with new value.
  Attribute attr_arg(arg, attr_.type, attr_.value_type);
  // Place the object in the vactor and add to the request for the Communicator.
  auto attr_args = std::vector<Attribute *>{&owner_identifier_, &attr_arg};
  ComControl::get_communicator().challenge(cmd_set_, Message::Flags::NONE,
                                           &attr_args, NULL);
  // Check if error callback reported error.
  if (ComControl::get_global_error_report() != "") {
    throw Exception("Property:set:error report after challenge call");
  }
}

template class Property<uint32_t>;
template class Property<std::string>;

}  // namespace wiphynlcontrol