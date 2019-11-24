#include "ComControl.h"

namespace wiphynlcontrol {
std::unique_ptr<Communicator> ComControl::com_ =
    std::make_unique<Communicator>(CALLBACK_DEFAULT);

Communicator &ComControl::get_communicator() { return *com_; }

void ComControl::set_global_callback_kind(const CallbackKind &kind) {
  com_->set_callback_kind(kind);
}

const std::string &ComControl::get_global_error_report() {
  return com_->get_error_report();
}

}  // namespace wiphynlcontrol