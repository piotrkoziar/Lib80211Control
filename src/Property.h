#ifndef WIPHYNLCONTROL_PROPERTY_H_
#define WIPHYNLCONTROL_PROPERTY_H_

#include <linux/nl80211.h>

#include "Attribute.h"

typedef enum nl80211_commands Nl80211Commands;

namespace wiphynlcontrol {

//  Typename should be std::string or uint32_t.
template <typename T>
class Property {
  friend class Entity;

 private:
  Attribute attr_;
  const Attribute *owner_identifier_;
  const Nl80211Commands cmd_get_;
  const Nl80211Commands cmd_set_;

 public:
  explicit Property(const Attribute *owner_id,
                    const Nl80211AttributeTypes &type,
                    const Attribute::ValueTypes &value_type,
                    const Nl80211Commands &cmd_get,
                    const Nl80211Commands &cmd_set);

  Attribute &get_attribute();

  const T &get_value() const;
  void set_value(T val);
  // In cases below, query the kernel.
  const T &get();
  void set(const T &arg);
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_PROPERTY_H_