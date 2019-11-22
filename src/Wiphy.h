#ifndef WIPHYNLCONTROL_WIPHY_H_
#define WIPHYNLCONTROL_WIPHY_H_

#include <string>
#include "Entity.h"

namespace wiphynlcontrol {

class Wiphy : public Entity {
 public:
  explicit Wiphy(const uint32_t &id);

 public:
  Property index_;
  Property name_;

 public:
  // Returns identifier. Overloads abstract method from Entity.
  const Attribute &get_identifier() const;
  // Uses Communicator to query the kernel.
  void set_name(Communicator &com);
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_WIPHY_H_