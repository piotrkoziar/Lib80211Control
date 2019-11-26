
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
    MATCH = NLM_F_MATCH,
    DUMP  = NLM_F_DUMP
  };

 private:
  LibnlMessage *message_;
  Flags flags_;

 public:
  explicit Message(const Flags &flags);

 public:
  LibnlMessage *get_message() const;
  const Flags &get_flags() const;
  void set_flags(const Flags &flags);

 public:
  ~Message();
};  // Message

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_MESSAGE_H_