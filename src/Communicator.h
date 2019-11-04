
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
  NlMessage       *message_;
  int             message_flags_;
  Nl80211Commands command_;
  AttributeSet    *attribute_set_;
  LibnlCallback   *callback_;
  std::string     error_report_;

 public:
  Communicator();
  Communicator(Entity *entity, Entity::Commands cmd, void **arg);

 private:
  // Sets attribute_set_ pointer on attribute set address from the entity.
  // Suitable attribute set is chosen based on the command.
  // Calls resolve_command() and resolve_attribute_set() from the Entity class.
  void get_attribute_set(Entity *entity, Entity::Commands cmd);
  // Adds attribute to the message_.
  void add_attribute(Nl80211AttributeTypes attr_type,
                     Entity::AttributeValueTypes attr_val_type,
                     void *attr_value);

 public:
  // Gets attributes from the message_.
  // AttributeBlocks from the attribute_set_ are used.
  // Returns immediately if attribute_set_ is not set (in most cases,
  // when the prepare_message() was not called).
  void get_attributes();
  // Prepares the message.
  // Calls get_attribute_set() and add_attribute().
  // Sets message_flags_. Adds identifier attributes to the message.
  // Usually called before the challenge() method.
  void prepare_message(Entity *entity, Entity::Commands cmd, void **arg);
  // Sends the message_ and gets the answer.
  void challenge(Socket *socket);
};  // class Communicator

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_COMMUNICATOR_H_