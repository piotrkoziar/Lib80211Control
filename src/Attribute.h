#ifndef WIPHYNLCONTROL_ATTRIBUTE_H_
#define WIPHYNLCONTROL_ATTRIBUTE_H_

#include <linux/nl80211.h>

#include <string>
#include <variant>

typedef enum nl80211_attrs Nl80211AttributeTypes;
typedef struct nlattr LibnlAttribute;

namespace wiphynlcontrol {

struct Attribute {
  enum class ValueTypes { UINT8, UINT32, UINT48, STRING };
  std::variant<std::string, uint8_t, uint32_t> value;
  const Nl80211AttributeTypes type;
  const ValueTypes value_type;
  Attribute(const std::variant<std::string, uint8_t, uint32_t> &val,
            const Nl80211AttributeTypes &tp,
            const ValueTypes &val_type);
};

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_ATTRIBUTE_H_
