
#include "Message.h"

#include <netlink/msg.h>

#include "Exception.h"

namespace wiphynlcontrol {

Message::Message() {
  message_ = nlmsg_alloc();
  if (!message_) {
    throw Exception("Message:Message:nlmsg_alloc:msg alloc failed");
  }
}

LibnlMessage *Message::get_message() const { return message_; }

Message::~Message() { nlmsg_free(message_); }

}  // namespace wiphynlcontrol