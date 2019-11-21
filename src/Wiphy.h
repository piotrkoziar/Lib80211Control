#ifndef WIPHYNLCONTROL_WIPHY_H_
#define WIPHYNLCONTROL_WIPHY_H_

#include <string>
#include "Entity.h"

namespace wiphynlcontrol {

class Wiphy : public Entity {
  // For better appearance.
  using sp_uint32 = std::shared_ptr<uint32_t>;
  using sp_string = std::shared_ptr<std::string>;

 public:
  explicit Wiphy(const uint32_t &id);

 public:
  sp_uint32 id_;
  sp_string name_;
  sp_uint32 bands_;
  sp_uint32 txq_params_;
  sp_uint32 frequency_;
  sp_uint32 channel_type_;

 public:
  // Returns identifier. Overloads abstract method from Entity.
  const std::unique_ptr<Attribute> get_identifier() const;
  // Uses Communicator to query the kernel.
  void set_name(Communicator &com);
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_WIPHY_H_