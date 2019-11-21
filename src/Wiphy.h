#ifndef WIPHYNLCONTROL_WIPHY_H_
#define WIPHYNLCONTROL_WIPHY_H_

#include <string>
#include "Entity.h"

namespace wiphynlcontrol {

class Wiphy : public Entity {
 public:
  explicit Wiphy(const uint32_t &id);

 public:
  std::shared_ptr<uint32_t>    id_;
  std::shared_ptr<std::string> name_;
  std::shared_ptr<std::string> address_;
  std::shared_ptr<uint32_t>    frequency_;

 public:
  // Returns identifier. Overloads abstract method from Entity.
  const std::unique_ptr<Attribute> get_identifier() const;
  // Uses Communicator to query the kernel.
  void set_name(Communicator &com);
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_WIPHY_H_