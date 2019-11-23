#include "Attribute.h"

namespace wiphynlcontrol {

Attribute::Attribute(const std::variant<std::string, uint32_t> &val,
                     const Nl80211AttributeTypes &type,
                     const ValueTypes &val_type)
    : value(val), type(type), val_type(val_type) {}
}  // namespace wiphynlcontrol