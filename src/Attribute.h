#ifndef WIPHYNLCONTROL_ATTRIBUTE_H_
#define WIPHYNLCONTROL_ATTRIBUTE_H_

#include <linux/nl80211.h>

#include <string>

typedef enum nl80211_attrs Nl80211AttributeTypes;
typedef struct nlattr LibnlAttribute;

typedef struct NestedAttr {
  struct nlattr **attr;
  struct nla_policy *policy;
} NestedAttr;

namespace wiphynlcontrol {

struct Attribute {
  enum class ValueTypes { UINT32, UINT48, STRING, NESTED_BSS };
  void  *value;
  const Nl80211AttributeTypes type;
  const ValueTypes value_type;
  void *parent;
  Attribute(void *val,
            const Nl80211AttributeTypes &tp,
            const ValueTypes &val_type,
            const Attribute *parent);
};

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_ATTRIBUTE_H_