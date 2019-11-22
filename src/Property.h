#ifndef WIPHYNLCONTROL_ATTRIBUTE_H_
#define WIPHYNLCONTROL_ATTRIBUTE_H_

#include <linux/nl80211.h>

#include <memory>
#include <variant>

typedef enum nl80211_commands Nl80211Commands;
typedef enum nl80211_attrs Nl80211AttributeTypes;
typedef struct nlattr LibnlAttribute;

namespace wiphynlcontrol {

struct Attribute {
  enum class ValueTypes { UINT32, STRING };
  std::variant<std::string, uint32_t> value;
  const Nl80211AttributeTypes type;
  const ValueTypes val_type;
  Attribute(const std::variant<std::string, uint32_t> &value,
            const Nl80211AttributeTypes &type, const ValueTypes &val_type);
};

template <typename T>
class Property {
 public:
  Attribute attr_;
  const Nl80211Commands cmd_;

 public:
  Property(const Nl80211AttributeTypes &type,
           const Attribute::ValueTypes &val_type, const Nl80211Commands &cmd);

  const T &get_value() const;
  void set_value(T val);
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_ATTRIBUTE_H_