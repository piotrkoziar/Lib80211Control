#ifndef WIPHYNLCONTROL_ATTRIBUTE_H_
#define WIPHYNLCONTROL_ATTRIBUTE_H_

#include <linux/nl80211.h>

#include <string>

typedef enum nl80211_attrs Nl80211AttributeTypes;
typedef struct nlattr LibnlAttribute;

namespace wiphynlcontrol {

typedef struct SSIDInfo {
  std::string ssid;
  uint32_t frequency;
  std::string mac_address;
  std::string status;
} SSIDInfo;

struct Attribute {
  enum class ValueTypes { UINT32, UINT48, STRING, NESTED, SCAN };
  void  *value;
  const Nl80211AttributeTypes type;
  const ValueTypes value_type;
  const Attribute *parent;
  Attribute(void *val,
            const Nl80211AttributeTypes &tp,
            const ValueTypes &val_type,
            const Attribute *par);
};

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_ATTRIBUTE_H_