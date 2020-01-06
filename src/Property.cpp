#include "Property.h"

#include <string>
#include <unordered_set>
#include <vector>

#include "ComControl.h"
#include "Exception.h"
#include "Message.h"

namespace wiphynlcontrol {

static const std::unordered_set<Nl80211Commands> dump_flag_required = {
  NL80211_CMD_GET_SCAN,
  NL80211_CMD_GET_STATION
};

template <typename T>
Property<T>::Property(const Attribute *owner_id,
                      const Nl80211AttributeTypes &type,
                      const Attribute::ValueTypes &value_type,
                      const Nl80211Commands &cmd_get,
                      const Nl80211Commands &cmd_set,
                      const Attribute *parent)
    : value_(T()),
      attr_(&value_, type, value_type, parent),
      owner_identifier_(owner_id),
      cmd_get_(cmd_get),
      cmd_set_(cmd_set) {}

template <typename T>
const T &Property<T>::get() {
  const auto attr_args = std::vector<const Attribute *>{owner_identifier_};
  const auto attr_read = std::vector<Attribute *>{&attr_};
  if (dump_flag_required.count(cmd_get_)) {  // Must have DUMP flag.
    ComControl::get_communicator().challenge(cmd_get_, Message::Flags::DUMP,
                                             &attr_args, &attr_read);
  } else {
    ComControl::get_communicator().challenge(cmd_get_, Message::Flags::NONE,
                                             &attr_args, &attr_read);
  }
  if (ComControl::get_global_error_report() != "") {
    throw Exception("Property:set:error report after challenge call");
  }
  return value_;
}

template <typename T>
void Property<T>::set(const T &arg) {
  // Create attribute object with new value.
  Attribute attr_arg(const_cast<T *>(&arg), attr_.type, attr_.value_type, NULL);
  // Place the object in the vactor and add to the request for the Communicator.
  const auto attr_args =
      std::vector<const Attribute *>{owner_identifier_, &attr_arg};
  ComControl::get_communicator().challenge(
      cmd_set_, Message::Flags::NONE, &attr_args, NULL);
  // Check if error callback reported error.
  if (ComControl::get_global_error_report() != "") {
    throw Exception("Property:set:error report after challenge call");
  }
}

template class Property<uint32_t>;
template class Property<std::string>;
template class Property<char>;
template class Property<std::vector<BSSInfo>>;
template class Property<uint64_t>;
template class Property<uint16_t>;
template class Property<int8_t>;
template class Property<bool>;
template class Property<nested_t>;

}  // namespace wiphynlcontrol