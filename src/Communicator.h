
#ifndef WIPHYNLCONTROL_COMMUNICATOR_H_
#define WIPHYNLCONTROL_COMMUNICATOR_H_

#include <vector>
#include "Attribute.h"
#include "Message.h"
#include "Socket.h"

typedef enum nl80211_commands Nl80211Commands;

namespace wiphynlcontrol {

class Communicator {
 private:
  int nl80211_family_id_;
  CallbackKind socket_cb_kind_;
  LibnlCallback *callback_;
  std::string error_report_;

 public:
  explicit Communicator(const CallbackKind &cb_kind = CALLBACK_DEFAULT);

 private:
  // Adds attribute to the message. Use before requesting the kernel.
  void add_attribute(LibnlMessage *message,
                     const std::unique_ptr<Attribute> &attr);
  // Uses the socket to query the kernel for numeric identifier of the
  // Generic Netlink family name. Sets nl80211_family_id_ with the result.
  void set_family_id(LibnlSocket *socket);
  // Sends the message_ and gets the answer.
  void send_and_receive(
      LibnlSocket *socket, LibnlMessage *message,
      const std::vector<std::unique_ptr<Attribute>> &attr_read);
  // Gets attributes from the message. Use as callback.
  static int get_attributes(
      LibnlMessage *msg,
      const std::vector<std::unique_ptr<Attribute>> &attr_read);

 public:
  // Prepares message and socket. Uses given command and
  // attributes to request and get the answer from the kernel.
  // @param attr_arg - attribute attached to the request. Usually the
  // identifier.
  // @param attr_read - attributes expected to be present in kernel's response.
  void challenge(const Nl80211Commands &command, const Message::Flags &flags,
                 const std::unique_ptr<Attribute> &attr_arg,
                 const std::vector<std::unique_ptr<Attribute>> &attr_read);

 public:
  ~Communicator();
};  // class Communicator

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_COMMUNICATOR_H_