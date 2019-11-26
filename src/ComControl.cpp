#include "ComControl.h"
#ifndef COM_DEBUG
#define COM_DEBUG 0
#endif

namespace wiphynlcontrol {
std::unique_ptr<Communicator> ComControl::com_ =
#if COM_DEBUG == 1
    std::make_unique<Communicator>(CALLBACK_DEBUG);
#else
    std::make_unique<Communicator>(CALLBACK_DEFAULT);
#endif
Communicator &ComControl::get_communicator() { return *com_; }

void ComControl::set_global_callback_kind(const CallbackKind &kind) {
  com_->set_callback_kind(kind);
}

const std::string &ComControl::get_global_error_report() {
  return com_->get_error_report();
}

}  // namespace wiphynlcontrol