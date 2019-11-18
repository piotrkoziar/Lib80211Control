
#include "Message.h"

#include <netlink/msg.h>

#include "Exception.h"

namespace wiphynlcontrol {

Message::Message(Flags flags) : flags_(flags) {
  message_ = nlmsg_alloc();
  if (!message_) {
    throw Exception("Message:Message:nlmsg_alloc:msg alloc failed");
  }
}

LibnlMessage *Message::get_message() { return message_; }

Message::Flags Message::get_flags() { return flags_; }

void Message::set_flags(Flags flags) { flags_ = flags; }

Message::~Message() { nlmsg_free(message_); }

}  // namespace wiphynlcontrol