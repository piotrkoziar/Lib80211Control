#ifndef WIPHYNLCONTROL_COMCONTROL_H_
#define WIPHYNLCONTROL_COMCONTROL_H_

#include "Communicator.h"

#include <memory>

namespace wiphynlcontrol {

class ComControl {
 private:
  static std::unique_ptr<Communicator> com;

 public:
  static Communicator &get_communicator();
  static void set_global_callback_kind(const CallbackKind &kind);
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_COMCONTROL_H_