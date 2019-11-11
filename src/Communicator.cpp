#include "Communicator.h"

#include <netlink/genl/genl.h>
#include <netlink/msg.h>
#include <netlink/netlink.h>

#include <iostream>
#include "Exception.h"

typedef struct nlmsghdr NlMessageHeader;

namespace wiphynlcontrol {

// Gets the error attribute from the message.
// Assigns charater string to the arg parameter.
static int error_handler(NlSocketAddress *nla, NlErrorMessageHeader *err,
                         char *arg);
static int ack_handler(NlMessage *msg, void *arg);
static int finish_handler(NlMessage *msg, void *arg);
// Calls get_attributes() from the Communicator object.
// Immediately returns if arg is NULL.
static int valid_handler(NlMessage *msg, Communicator *arg);

Communicator::Communicator(CallbackKind cb_kind) {
  message_ = nlmsg_alloc();
  if (!message_) {
    throw Exception("Message(): message allocation failed");
  }

  callback_ = nl_cb_alloc(static_cast<LibnlCallbackKind>(cb_kind));
  if (!callback_) {
    throw Exception("Message(): callback allocation failed");
  }
}

Communicator::Communicator(CallbackKind cb_kind, Entity *attr,
                           Entity::Commands cmd, void **arg) {
  Communicator();
  // TODO
}

void Communicator::get_attribute_set(Entity *entity, Entity::Commands cmd) {
  command_ = entity->resolve_command(cmd);
  attribute_set_ = entity->resolve_attribute_set(cmd);
}

void Communicator::add_attribute(Nl80211AttributeTypes attr_type,
                                 Entity::AttributeValueTypes attr_val_type,
                                 void *attr_value) {
  switch (attr_val_type) {
    case Entity::AttributeValueTypes::UINT32:
      NLA_PUT_U32(message_, attr_type, *(static_cast<uint32_t *>(attr_value)));
      break;

    case Entity::AttributeValueTypes::STRING:
      NLA_PUT_STRING(message_, attr_type, static_cast<char *>(attr_value));
      break;

    default:
    nla_put_failure:  // Needed by NLA_PUT macro.
      break;
  }
}

void Communicator::get_attributes() {
  if (!message_ || !attribute_set_) {
    return;
  }

  NlGeMessageHeader *header;
  NlAttribute *attributes[NL80211_ATTR_MAX + 1];

  if (!(header = static_cast<NlGeMessageHeader *>(
            nlmsg_data(nlmsg_hdr(message_))))) {
    return;
  }

  nla_parse(attributes, NL80211_ATTR_MAX, genlmsg_attrdata(header, 0),
            genlmsg_attrlen(header, 0), NULL);

  void *attribute_value;
  for (auto it = attribute_set_->begin(); it != attribute_set_->end(); ++it) {
    if (attributes[it->attr_type]) {
      attribute_value = nla_data(attributes[it->attr_type]);
      switch (it->attr_val_type) {
        default:
        case Entity::AttributeValueTypes::UINT32:
          *static_cast<uint32_t *>(it->attr_class_member) =
              *static_cast<uint32_t *>(attribute_value);
          break;

        case Entity::AttributeValueTypes::STRING:
          *static_cast<std::string *>(it->attr_class_member) =
              *static_cast<std::string *>(attribute_value);
      }
    }
  }
}

void Communicator::prepare_message(Entity *entity, Entity::Commands cmd,
                                   void **arg) {
  if (!entity) {
    throw Exception("prepare_message(): argument is NULL");
  }

  Entity::AttributeBlock *identifier = entity->get_identifier(arg);

  get_attribute_set(entity, cmd);

  message_flags_ = NLM_F_DUMP;

  add_attribute(identifier->attr_type, identifier->attr_val_type,
                identifier->attr_class_member);
}

void Communicator::challenge(Socket *socket) {
  if (!socket) {
    throw Exception("Communicator::challenge(): argument is NULL");
  }
  if (!command_ || !message_flags_ || !message_) {
    throw Exception("Communicator::challenge(): class members not set");
  }

  // Add header to the message
  if (!genlmsg_put(message_,                 // message
                   NL_AUTO_PORT,             // port (auto)
                   NL_AUTO_SEQ,              // sequence (auto)
                   socket->get_family_id(),  // family (nl80211) id
                   0,                        // user header len
                   message_flags_,           // message flags
                   command_,                 // command
                   0))                       // interface version
  {
    throw Exception("Communicator::challenge(): adding message header failed");
  }

  // Send the message
  if (nl_send_auto(socket->get_socket(), message_) < 0) {
    throw Exception(
        "Communicator::challenge(): nl_send_auto() exited with negative error "
        "code");
  }

  // Set up callbacks
  int ret = 1;
  nl_cb_err(callback_, NL_CB_CUSTOM,
            reinterpret_cast<nl_recvmsg_err_cb_t>(error_handler),
            &this->error_report_);
  nl_cb_set(callback_, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &ret);
  nl_cb_set(callback_, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &ret);
  nl_cb_set(callback_, NL_CB_VALID, NL_CB_CUSTOM,
            reinterpret_cast<nl_recvmsg_msg_cb_t>(valid_handler), this);

  // Get the answer.
  nl_recvmsgs(socket->get_socket(), callback_);
}

Communicator::~Communicator() {
  nl_cb_put(callback_);
  nlmsg_free(message_);
}

// Message handlers.

int error_handler(NlSocketAddress *nla, NlErrorMessageHeader *err_msg,
                  char *arg) {
  /* Example:

  [NETLINK HEADER] 16 octets <---------header
          .nlmsg_len = 36
          .type = 2 <ERROR>
          .flags = 256 <ROOT>
          .seq = 1572300661
          .port = -327123043
  [ERRORMSG] 20 octets <---------err_msg
          .error = 0 "Success"
  [ORIGINAL MESSAGE] 16 octets
          .nlmsg_len = 16
          .type = 16 <0x10>
          .flags = 5 <REQUEST,ACK>
          .seq = 1572300661
          .port = -327123043
  */

  if (!arg) {
    return NL_STOP;
  }

  if (err_msg->error == 0) {
    // Success.
    return NL_STOP;
  }

  if (!(err_msg->msg.nlmsg_flags & NLM_F_ACK_TLVS)) {
    // There is no error attributes in the message. Stop.
    return NL_STOP;
  }

  // Resolve length of the message.
  uint32_t ack_length;
  if (!(err_msg->msg.nlmsg_flags & NLM_F_CAPPED)) {
    ack_length = sizeof(NlMessageHeader) + sizeof(int) + err_msg->msg.nlmsg_len;
  } else {
    ack_length =
        sizeof(NlMessageHeader) + sizeof(int) + sizeof(NlMessageHeader);
  }

  NlMessageHeader *header = reinterpret_cast<NlMessageHeader *>(err_msg) - 1;
  if (header->nlmsg_len <= ack_length) {
    // There is no attributes.
    return NL_STOP;
  }
  // Start address of the error attributes.
  NlAttribute *attributes = reinterpret_cast<NlAttribute *>(
      reinterpret_cast<unsigned char *>(header) + ack_length);
  // The error attributes addressess are stored in the table.
  NlAttribute *err_attributes[NLMSGERR_ATTR_MAX + 1];
  nla_parse(err_attributes, NLMSGERR_ATTR_MAX, attributes,
            (header->nlmsg_len - ack_length), NULL);
  // Try to get specific error attribute.
  if (err_attributes[NLMSGERR_ATTR_MSG]) {
    arg = static_cast<char *>(nla_data(err_attributes[NLMSGERR_ATTR_MSG]));
  }
  return NL_STOP;
}
int ack_handler(nl_msg *msg, void *arg) {
  *static_cast<int *>(arg) = 0;
  return NL_STOP;
}
int finish_handler(NlMessage *msg, void *arg) {
  *static_cast<int *>(arg) = 0;
  return NL_SKIP;
}
int valid_handler(NlMessage *msg, Communicator *arg) {
  if (!arg) {
    return NL_OK;
  }
  static_cast<Communicator *>(arg)->get_attributes();
  return NL_OK;
}

}  // namespace wiphynlcontrol