#ifndef WIPHYNLCONTROL_ENTITY_H_
#define WIPHYNLCONTROL_ENTITY_H_

#include <memory>
#include "Communicator.h"
#include "Property.h"

namespace wiphynlcontrol {

// This is parent class for the Wiphy, Interface etc. classes.
class Entity {
 public:
  // This member identifies the Entity.
  virtual const uint32_t &get_identifier() const = 0;
  virtual void set_identifier(const uint32_t &id) = 0;
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_ENTITY_H_