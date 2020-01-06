#include "Attribute.h"

namespace wiphynlcontrol {

Attribute::Attribute(void *val,
                     const Nl80211AttributeTypes &tp,
                     const ValueTypes &val_type,
                     const Attribute *par)
    : value(val), type(tp), value_type(val_type), parent(par) {}
}  // namespace wiphynlcontrol