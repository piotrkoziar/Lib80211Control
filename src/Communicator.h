
#ifndef WIPHYNLCONTROL_COMMUNICATOR_H_
#define WIPHYNLCONTROL_COMMUNICATOR_H_

#include "Wiphy.h"
#include "Socket.h"

typedef struct genlmsghdr NlGeMessageHeader;
typedef struct nl_msg NlMessage;
typedef struct sockaddr_nl NlSocketAddress;
typedef struct nlmsgerr NlErrorMessageHeader;

namespace wiphynlcontrol {

typedef std::set<Entity::AttributeBlock> AttributeSet;

class Communicator {
 private:
  int             nl80211_family_id_;
  NlMessage       *message_;
  int             message_flags_;
  Nl80211Commands command_;
  AttributeSet    *attribute_set_;
  LibnlCallback   *callback_;
  std::string     error_report_;

 public:
  Communicator(CallbackKind cb_kind = CALLBACK_DEFAULT);
  Communicator(CallbackKind cb_kind, Entity *entity, Entity::Commands cmd,
               void **arg);

 private:
  // Sets attribute_set_ pointer on attribute set address from the entity.
  // Suitable attribute set is chosen based on the command.
  // Calls resolve_command() and resolve_attribute_set() from the Entity class.
  void get_attribute_set(Entity *entity, Entity::Commands cmd);
  // Adds attribute to the message_.
  void add_attribute(Nl80211AttributeTypes attr_type,
                     Entity::AttributeValueTypes attr_val_type,
                     void *attr_value);
  // Uses the socket to query the kernel for numeric identifier of the Generic
  // Netlink family name. Sets nl80211_family_id_ with the result.
  void set_family_id(Socket *socket);
  // Sends the message_ and gets the answer.
  void send_and_receive(Socket *socket);

 public:
  // Gets attributes from the message_.
  // AttributeBlocks from the attribute_set_ are used.
  // Returns immediately if attribute_set_ is not set.
  void get_attributes(NlMessage *msg);
  // Prepares the message.
  // Calls get_attribute_set() and add_attribute().
  // Sets message_flags_. Adds identifier attributes to the message.
  void challenge(Socket *socket, Entity *entity, Entity::Commands cmd,
                 void **arg);

 public:
  ~Communicator();
};  // class Communicator

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_COMMUNICATOR_H_