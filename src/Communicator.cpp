#include "Communicator.h"

#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/netlink.h>

#include <iostream>
#include <memory>
#include "Exception.h"

typedef struct nlmsghdr LibnlMessageHeader;
typedef struct nlmsgerr LibnlErrorMessageHeader;

namespace wiphynlcontrol {

// Gets the error attribute from the message.
// Assigns charater string to the arg parameter.
static int error_handler(NlSocketAddress *nla, LibnlErrorMessageHeader *err,
                         char *arg);
static int ack_handler(LibnlMessage *msg, void *arg);
static int finish_handler(LibnlMessage *msg, void *arg);

Communicator::Communicator(CallbackKind cb_kind) : socket_cb_kind_(cb_kind) {
  // Allocate callback_
  callback_ = nl_cb_alloc(static_cast<LibnlCallbackKind>(cb_kind));
  if (!callback_) {
    throw Exception("Communicator: callback allocation failed");
  }
  // Resolve family id
  auto socket = std::make_unique<Socket>(CALLBACK_DEFAULT);
  set_family_id(socket->get_socket());
}

void Communicator::add_attribute(LibnlMessage *message,
                                 std::weak_ptr<Entity::Attribute> attr_arg) {
  std::shared_ptr<Entity::Attribute> attr = attr_arg.lock();
  if (!attr) {
    return;  // Weak pointer points to dead object.
  }

  switch (attr->val_type_) {
    case Entity::Attribute::ValueTypes::UINT32: {
      if (std::shared_ptr<uint32_t> attr_value =
              std::static_pointer_cast<uint32_t>(attr->class_member_.lock())) {
        NLA_PUT_U32(message, attr->type_, *attr_value);
      }
      break;
    }
    case Entity::Attribute::ValueTypes::STRING: {
      if (std::shared_ptr<std::string> attr_value =
              std::static_pointer_cast<std::string>(
                  attr->class_member_.lock())) {
        NLA_PUT_STRING(message, attr->type_, (*attr_value).c_str());
      };
      break;
    }
    default: {
    nla_put_failure:  // Needed by NLA_PUT macro.
      break;
    }
  }
}

void Communicator::set_family_id(LibnlSocket *socket) {
  if ((nl80211_family_id_ = genl_ctrl_resolve(socket, "nl80211")) < 0) {
    throw Exception("Communicator::set_family_id() failed");
  }
}

void Communicator::send_and_receive(
    LibnlSocket *socket, LibnlMessage *message,
    std::unique_ptr<std::vector<Entity::Attribute>> attr_read) {
  if (!socket || !message) {
    throw Exception("Communicator::send_and_receive(): argument is NULL");
  }

  // Send the message
  if (nl_send_auto(socket, message) < 0) {
    throw Exception(
        "Communicator::send_and_receive() nl_send_auto() exited with negative \
        error code");
  }

  // Set up callbacks
  int ret = 1;
  nl_cb_err(callback_, NL_CB_CUSTOM,
            reinterpret_cast<nl_recvmsg_err_cb_t>(error_handler),
            &this->error_report_);
  nl_cb_set(callback_, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &ret);
  nl_cb_set(callback_, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &ret);
  nl_cb_set(callback_, NL_CB_VALID, NL_CB_CUSTOM,
            reinterpret_cast<nl_recvmsg_msg_cb_t>(get_attributes),
            attr_read.get());

  // Get the answer.
  nl_recvmsgs(socket, callback_);
}

int Communicator::get_attributes(
    LibnlMessage *msg, const std::vector<Entity::Attribute> &attr_read) {
  // Get message header
  NlGeMessageHeader *header;
  if (!(header =
            static_cast<NlGeMessageHeader *>(nlmsg_data(nlmsg_hdr(msg))))) {
    return NL_OK;
  }
  // Get message attributes
  LibnlAttribute *attributes[NL80211_ATTR_MAX + 1];
  nla_parse(attributes, NL80211_ATTR_MAX, genlmsg_attrdata(header, 0),
            genlmsg_attrlen(header, 0), NULL);

  void *attribute_value;
  for (auto it = attr_read.begin(); it != attr_read.end(); ++it) {
    if (attributes[it->type_]) {
      attribute_value = nla_data(attributes[it->type_]);
      switch (it->val_type_) {
        default:
        case Entity::Attribute::ValueTypes::UINT32:
          *std::static_pointer_cast<uint32_t>(it->class_member_.lock()) =
              *static_cast<uint32_t *>(attribute_value);
          break;

        case Entity::Attribute::ValueTypes::STRING:
          *std::static_pointer_cast<std::string>(it->class_member_.lock()) =
              static_cast<const char *>(attribute_value);
          break;
      }
    }
  }
  return NL_OK;
}

void Communicator::challenge(
    const Nl80211Commands command, const Message::Flags flags,
    std::weak_ptr<Entity::Attribute> attr_arg,
    std::unique_ptr<std::vector<Entity::Attribute>> attr_read) {
  auto socket = std::make_unique<Socket>(socket_cb_kind_);
  auto message = std::make_unique<Message>(flags);

  // Add Netlink header, Generic Netlink header to the message.
  if (!genlmsg_put(message->get_message(),  // message
                   NL_AUTO_PORT,            // port (auto)
                   NL_AUTO_SEQ,             // sequence (auto)
                   nl80211_family_id_,      // family (nl80211) id
                   0,                       // user header len
                   static_cast<int>(message->get_flags()),  // message flags
                   command,                                 // command
                   0))                                      // interface version
  {
    throw Exception("Communicator::challenge(): adding message header failed");
  }

  add_attribute(message->get_message(), attr_arg);

  send_and_receive(socket->get_socket(), message->get_message(),
                   std::move(attr_read));
}

Communicator::~Communicator() { nl_cb_put(callback_); }

// Message handlers.

int error_handler(NlSocketAddress *nla, LibnlErrorMessageHeader *err_msg,
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
    ack_length =
        sizeof(LibnlMessageHeader) + sizeof(int) + err_msg->msg.nlmsg_len;
  } else {
    ack_length =
        sizeof(LibnlMessageHeader) + sizeof(int) + sizeof(LibnlMessageHeader);
  }

  LibnlMessageHeader *header =
      reinterpret_cast<LibnlMessageHeader *>(err_msg) - 1;
  if (header->nlmsg_len <= ack_length) {
    // There is no attributes.
    return NL_STOP;
  }
  // Start address of the error attributes.
  LibnlAttribute *attributes = reinterpret_cast<LibnlAttribute *>(
      reinterpret_cast<unsigned char *>(header) + ack_length);
  // The error attributes addressess are stored in the table.
  LibnlAttribute *err_attributes[NLMSGERR_ATTR_MAX + 1];
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
int finish_handler(LibnlMessage *msg, void *arg) {
  *static_cast<int *>(arg) = 0;
  return NL_SKIP;
}

}  // namespace wiphynlcontrol