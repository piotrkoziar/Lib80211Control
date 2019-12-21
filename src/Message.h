
#ifndef WIPHYNLCONTROL_MESSAGE_H_
#define WIPHYNLCONTROL_MESSAGE_H_

#include <linux/netlink.h>

typedef struct nl_msg LibnlMessage;

namespace wiphynlcontrol {

class Message {
 public:
  enum class Flags {
    NONE  = 0,
    ROOT  = NLM_F_ROOT,
    DUMP  = NLM_F_DUMP
  };

 private:
  LibnlMessage *message_;

 public:
  explicit Message();

 public:
  LibnlMessage *get_message() const;

 public:
  ~Message();
};  // Message

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_MESSAGE_H_
