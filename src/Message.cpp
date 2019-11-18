
#include "Message.h"

#include <netlink/msg.h>

#include "Exception.h"

namespace wiphynlcontrol {

Message::Message(const Flags &flags) : flags_(flags) {
  message_ = nlmsg_alloc();
  if (!message_) {
    throw Exception("Message:Message:nlmsg_alloc:msg alloc failed");
  }
}

LibnlMessage *Message::get_message() const { return message_; }

const Message::Flags &Message::get_flags() const { return flags_; }

void Message::set_flags(const Flags &flags) { flags_ = flags; }

Message::~Message() { nlmsg_free(message_); }

}  // namespace wiphynlcontrol