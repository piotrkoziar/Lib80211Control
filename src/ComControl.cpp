#include "ComControl.h"

namespace wiphynlcontrol {
std::unique_ptr<Communicator> ComControl::com =
    std::make_unique<Communicator>(CALLBACK_DEBUG);

Communicator &ComControl::get_communicator() { return *com; }

void ComControl::set_global_callback_kind(const CallbackKind &kind) {
  com->set_callback_kind(kind);
}

}  // namespace wiphynlcontrol