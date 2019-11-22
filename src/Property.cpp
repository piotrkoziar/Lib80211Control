#include "Property.h"

namespace wiphynlcontrol {

Attribute::Attribute(const std::variant<std::string, uint32_t> &val,
                     const Nl80211AttributeTypes &type,
                     const ValueTypes &val_type)
    : value(val), type(type), val_type(val_type) {}

Property::Property(const Nl80211AttributeTypes &type,
                   const Attribute::ValueTypes &val_type,
                   const Nl80211Commands &cmd)
    : attr_(0, type, val_type), cmd_(cmd) {}

const Attribute &Property::get() const {
  return attr_;
}

}  // namespace wiphynlcontrol
