
#ifndef WIPHYNLCONTROL_MESSAGE_H_
#define WIPHYNLCONTROL_MESSAGE_H_

#include <linux/netlink.h>

#include <memory>

typedef struct nl_msg LibnlMessage;

namespace wiphynlcontrol {

class Message {
 public:
  enum class Flags {
    NONE  = 0,
    ROOT  = NLM_F_ROOT,
    MATCH = NLM_F_MATCH,
    DUMP  = NLM_F_DUMP
  };

 private:
  LibnlMessage *message_;
  Flags flags_;

 public:
  Message(Flags flags);

 public:
  // Returns pointer to the LibnlMessage member.
  LibnlMessage *get_message();
  // Returns flags.
  Flags get_flags();
  // Sets flags.
  void set_flags(Flags flags);

 public:
  ~Message();
};  // Message

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_MESSAGE_H_