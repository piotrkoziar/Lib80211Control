
#ifndef WIPHYNLCONTROL_COMMUNICATOR_H_
#define WIPHYNLCONTROL_COMMUNICATOR_H_

#include <vector>
#include "Message.h"
#include "Socket.h"
#include "Wiphy.h"

typedef struct genlmsghdr NlGeMessageHeader;
typedef struct sockaddr_nl NlSocketAddress;

namespace wiphynlcontrol {

class Communicator {
 private:
  int nl80211_family_id_;
  CallbackKind socket_cb_kind_;
  LibnlCallback *callback_;
  std::string error_report_;

 public:
  Communicator(CallbackKind cb_kind = CALLBACK_DEFAULT);

 private:
  // Adds attribute to the message_.
  void add_attribute(LibnlMessage *message,
                     std::weak_ptr<Entity::Attribute> attr_arg);
  // Uses the socket to query the kernel for numeric identifier of the
  // Generic Netlink family name. Sets nl80211_family_id_ with the result.
  void set_family_id(LibnlSocket *socket);
  // Sends the message_ and gets the answer.
  void send_and_receive(
      LibnlSocket *socket, LibnlMessage *message,
      std::unique_ptr<std::vector<Entity::Attribute>> attr_read);
  // Gets attributes from the message_.
  // AttributeBlocks from the attribute_set_ are used.
  // Returns immediately if attribute_set_ is not set.
  static int get_attributes(LibnlMessage *msg,
                            const std::vector<Entity::Attribute> &attr_read);

 public:
  // Prepares the message.
  // Calls get_attribute_set() and add_attribute().
  // Sets message_flags_. Adds identifier attributes to the message.
  void challenge(const Nl80211Commands command, const Message::Flags flags,
                 std::weak_ptr<Entity::Attribute> attr_arg,
                 std::unique_ptr<std::vector<Entity::Attribute>> attr_read);

 public:
  ~Communicator();
};  // class Communicator

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_COMMUNICATOR_H_