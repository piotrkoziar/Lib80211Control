#include "Attribute.h"

namespace wiphynlcontrol {

Attribute::Attribute(const std::weak_ptr<void> class_member,
                     const Nl80211AttributeTypes &type,
                     const ValueTypes &val_type)
    : class_member_(class_member), type_(type), val_type_(val_type) {}

}  // namespace wiphynlcontrol