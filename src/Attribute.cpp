#include "Entity.h"

namespace wiphynlcontrol {

Entity::Entity(const std::weak_ptr<void> &identifier,
               const Nl80211AttributeTypes &type,
               const Attribute::ValueTypes &val_type)
    : identifier_(std::make_shared<Attribute>(identifier, type, val_type)) {}

Entity::Attribute::Attribute(const std::weak_ptr<void> &class_member,
                             const Nl80211AttributeTypes &type,
                             const ValueTypes &val_type)
    : class_member_(class_member), type_(type), val_type_(val_type) {}

}  // namespace wiphynlcontrol