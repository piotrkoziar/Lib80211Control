#ifndef WIPHYNLCONTROL_ENTITY_H_
#define WIPHYNLCONTROL_ENTITY_H_

#include <linux/nl80211.h>

#include <memory>
#include <stdint.h>


typedef enum nl80211_commands Nl80211Commands;
typedef enum nl80211_attrs    Nl80211AttributeTypes;
typedef struct nlattr LibnlAttribute;

namespace wiphynlcontrol {

// This is parent class for the Wiphy, Interface etc. classes.
class Entity {
 public:
  class Attribute {
   public:
    enum class ValueTypes { UINT32, STRING };

   public:
    const std::weak_ptr<void> class_member_;
    const Nl80211AttributeTypes type_;
    const ValueTypes val_type_;

   public:
    explicit Attribute(const std::weak_ptr<void> &class_member,
                       const Nl80211AttributeTypes &type,
                       const ValueTypes &val_type);
  };

 protected:
  const std::shared_ptr<Attribute> identifier_;

 protected:
  Entity(const std::weak_ptr<void> &identifier,
         const Nl80211AttributeTypes &type,
         const Attribute::ValueTypes &val_type);

 public:
  // This member identifies the Entity.
  virtual const std::weak_ptr<Attribute> get_identifier() const = 0;
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_ENTITY_H_