#ifndef WIPHYNLCONTROL_ATTRIBUTE_H_
#define WIPHYNLCONTROL_ATTRIBUTE_H_

#include <linux/nl80211.h>

#include <memory>

typedef enum nl80211_attrs Nl80211AttributeTypes;
typedef struct nlattr LibnlAttribute;

namespace wiphynlcontrol {

class Attribute {
 public:
  enum class ValueTypes { UINT32, STRING };

 public:
  const std::weak_ptr<void> class_member_;
  const Nl80211AttributeTypes type_;
  const ValueTypes val_type_;

 public:
  Attribute(const std::weak_ptr<void> class_member,
            const Nl80211AttributeTypes &type, const ValueTypes &val_type);
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_ATTRIBUTE_H_