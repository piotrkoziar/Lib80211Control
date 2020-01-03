#include "Attribute.h"

namespace wiphynlcontrol {

Attribute::Attribute(void *val,
                     const Nl80211AttributeTypes &tp,
                     const ValueTypes &val_type,
                     const Attribute *parent)
    : value(val), type(tp), value_type(val_type) {}
}  // namespace wiphynlcontrol