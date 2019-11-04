#include "Entity.h"

namespace wiphynlcontrol {

Nl80211Commands Entity::resolve_command(Commands cmd) {
  return command_map_.at(cmd);
}

std::set<Entity::AttributeBlock> *Entity::resolve_attribute_set(
    Commands cmd) {
  return &attribute_map_.at(cmd);
}

}  // namespace wiphynlcontrol