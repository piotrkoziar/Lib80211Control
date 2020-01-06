#ifndef WIPHYNLCONTROL_ATTRIBUTE_H_
#define WIPHYNLCONTROL_ATTRIBUTE_H_

#include <linux/nl80211.h>

#include <string>

typedef enum nl80211_attrs Nl80211AttributeTypes;
typedef struct nlattr LibnlAttribute;

namespace wiphynlcontrol {

typedef struct BSSInfo {
  std::string ssid;
  uint32_t frequency;
  std::string bssid;
  std::string status;
} BSSInfo;

struct Attribute {
  enum class ValueTypes { UINT8, UINT16, UINT32, UINT48, STRING, NESTED, SCAN };
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