#ifndef WIPHYNLCONTROL_COMCONTROL_H_
#define WIPHYNLCONTROL_COMCONTROL_H_

#include <memory>

#include "Communicator.h"

namespace wiphynlcontrol {

class ComControl {
 private:
  static std::unique_ptr<Communicator> com_;

 public:
  static Communicator &get_communicator();
  static void set_global_callback_kind(const CallbackKind &kind);
  static const std::string &get_global_error_report();
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_COMCONTROL_H_