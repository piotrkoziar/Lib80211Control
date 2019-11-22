#ifndef WIPHYNLCONTROL_ENTITY_H_
#define WIPHYNLCONTROL_ENTITY_H_

#include <memory>
#include "Property.h"
#include "Communicator.h"

namespace wiphynlcontrol {

// This is parent class for the Wiphy, Interface etc. classes.
class Entity {
 public:
  // This member identifies the Entity.
  virtual const Attribute &get_identifier() const = 0;
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_ENTITY_H_