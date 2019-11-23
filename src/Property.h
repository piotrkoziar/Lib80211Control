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
  Attribute owner_identifier_;
  Attribute attr_;
  const Nl80211Commands cmd_;

 public:
  explicit Property(const Attribute &owner_id,
                    const Nl80211AttributeTypes &type,
                    const Attribute::ValueTypes &val_type,
                    const Nl80211Commands &cmd);

  const T &get_value() const;
  void set_value(T val);
  // In this case, query the kernel.
  const T &get();
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_PROPERTY_H_