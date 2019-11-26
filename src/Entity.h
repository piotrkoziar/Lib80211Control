#ifndef WIPHYNLCONTROL_ENTITY_H_
#define WIPHYNLCONTROL_ENTITY_H_

#include "Attribute.h"

namespace wiphynlcontrol {

template <typename T>
class Property;

// This is parent class for the Wiphy, Interface etc. classes.
class Entity {
 protected:
  // This function can be called by any derived class to access private
  // Property class member. Entity is a friend to the Property class.
  template <typename T>
  const Attribute &get_attributes(const Property<T> &prop) const;

 public:
  // This member identifies the Entity.
  virtual const uint32_t &get_identifier() const  = 0;
  virtual void set_identifier(const uint32_t &id) = 0;
};

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_ENTITY_H_