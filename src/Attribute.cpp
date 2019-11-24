#include "Attribute.h"

namespace wiphynlcontrol {

Attribute::Attribute(const std::variant<std::string, uint32_t> &val,
                     const Nl80211AttributeTypes &tp,
                     const ValueTypes &val_type)
    : value(val), type(tp), value_type(val_type) {}
}  // namespace wiphynlcontrol