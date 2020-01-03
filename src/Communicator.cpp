#include "Communicator.h"

#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/netlink.h>
#include <net/if.h>

#include <iostream>
#include <memory>
#include <memory>

#include "Exception.h"

typedef struct nlmsghdr LibnlMessageHeader;
typedef struct nlmsgerr LibnlErrorMessageHeader;
typedef struct genlmsghdr LibnlGeMessageHeader;
typedef struct sockaddr_nl LibnlSocketAddress;

namespace wiphynlcontrol {

// Gets the error attribute from the message.
// Assigns charater string to the arg parameter.
static int error_handler(LibnlSocketAddress *nla,
                         LibnlErrorMessageHeader *err,
                         std::string *arg);
static int ack_handler(LibnlMessage *msg, void *arg);
static int finish_handler(LibnlMessage *msg, void *arg);

Communicator::Communicator(const CallbackKind &cb_kind)
    : socket_cb_kind_(cb_kind) {
  callback_ = nl_cb_alloc(static_cast<LibnlCallbackKind>(cb_kind));
  if (!callback_) {
    throw Exception("Communicator:Communicator:callback allocation failed");
  }
  // Create socket to query the kernel through it.
  auto socket = std::make_unique<Socket>(CALLBACK_DEFAULT);
  set_family_id(socket->get_socket());
}

void Communicator::add_attributes(
    LibnlMessage *message, const std::vector<const Attribute *> *attr_arg) {
  if (!message) {
    throw Exception("Communicator:add_attribute:argument message is NULL");
  } else if (!attr_arg) {
    return;  // There is no attributes to add.
  }

  for (auto &it : *attr_arg) {
    switch (it->value_type) {
      case Attribute::ValueTypes::UINT32: {
        NLA_PUT_U32(message, it->type, *static_cast<uint32_t *>(it->value));
        break;
      }
      case Attribute::ValueTypes::STRING: {
        NLA_PUT_STRING(
            message, it->type, static_cast<std::string *>(it->value)->c_str());
        break;
      }
      default: {
      nla_put_failure:  // Needed by NLA_PUT macro.
        break;
      }
    }
  }
}

void Communicator::set_family_id(LibnlSocket *socket) {
  if (!socket) {
    throw Exception("Communicator:set_family_id:argument is NULL");
  }

  if ((nl80211_family_id_ = genl_ctrl_resolve(socket, "nl80211")) < 0) {
    throw Exception("Communicator:set_family_id:failed");
  }
}

void Communicator::send_and_receive(LibnlSocket *socket,
                                    LibnlMessage *message,
                                    const std::vector<Attribute *> *attr_read) {
  if (!socket || !message) {
    throw Exception("Communicator:send_and_receive:argument is NULL");
  }

  // Set up callbacks.
  int ret = 1;
  nl_cb_err(callback_,
            NL_CB_CUSTOM,
            reinterpret_cast<nl_recvmsg_err_cb_t>(error_handler),
            static_cast<void *>(&error_report_));
  nl_cb_set(callback_, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &ret);
  nl_cb_set(callback_, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &ret);
  nl_cb_set(callback_,
            NL_CB_VALID,
            NL_CB_CUSTOM,
            reinterpret_cast<nl_recvmsg_msg_cb_t>(get_attributes),
            const_cast<void *>(static_cast<const void *>(attr_read)));

  // Send the message
  if (nl_send_auto(socket, message) < 0) {
    throw Exception(
        "Communicator:send_and_receive:nl_send_auto: exited with negative \
        error code");
  }
  // Get the answer.
  nl_recvmsgs(socket, callback_);
}

int Communicator::get_attributes(LibnlMessage *msg,
                                 const std::vector<Attribute *> *attr_read) {
  void *attribute_to_read_value;
  int attribute_to_read_type;
  int attribute_type;  // Type of the attribute cast to int.
  int result;
  std::unique_ptr<LibnlAttribute *[]> parent_attributes, nested_attributes;
  static auto attributes =
      std::make_unique<LibnlAttribute *[]>(NL80211_ATTR_MAX + 1);

#ifdef COM_DEBUG
  std::cout << "Received ATTRs:\n";
  for (auto type = NL80211_ATTR_UNSPEC; type < NL80211_ATTR_MAX;
       type      = static_cast<Nl80211AttributeTypes>(type + 1)) {
    attribute_to_read_value = nla_data(attributes.get()[type]);
    if (reinterpret_cast<long>(attribute_to_read_value) != 0x4) {
      std::cout << int(type) << "\t" << attribute_to_read_value << std::endl;
    }
  }
#endif

  if (!attr_read) {
    return NL_OK;  // No attributes was specified to read from message.
  }
  // Get message header
  LibnlGeMessageHeader *header;
  if (!(header =
            static_cast<LibnlGeMessageHeader *>(nlmsg_data(nlmsg_hdr(msg))))) {
    return NL_OK;
  }
  // Get message attributes
  nla_parse(attributes.get(),
            NL80211_ATTR_MAX,
            genlmsg_attrdata(header, 0),
            genlmsg_attrlen(header, 0),
            NULL);

  const Attribute *attr_top;
  std::vector<const Attribute *> attr_vec;
  for (auto &attribute_to_read : *attr_read) {
    // Push all parents (from bottom to top) to the vector.
    attr_top = attribute_to_read;
    while (attr_top->parent) {
      attr_top = attr_top->parent;
      attr_vec.push_back(attr_top);
      std::cout << "Iteration in attr_top loop \n";
    }

    // Get the attribute value. The value is true only for not-nested
    // attributes.
    attribute_to_read_type = static_cast<int>(attribute_to_read->type);
    if ((attribute_to_read_type <= NL80211_ATTR_MAX) &&
        (attributes.get()[attribute_to_read_type])) {
      attribute_to_read_value =
          nla_data(parent_attributes[attribute_to_read_type]);
    }

    // Enter the loop only if nested.
    for (auto i = attr_vec.size(); i > 0; --i) {
      attribute_type = static_cast<int>(attr_vec[i - 1]->type);
      nested_attributes =
          std::make_unique<LibnlAttribute *[]>(attribute_type + 1);

      if (i == attr_vec.size()) {
        result = nla_parse_nested(nested_attributes.get(), attribute_type,
                                  attributes.get()[attribute_type], NULL);
      } else {
        result =
            nla_parse_nested(nested_attributes.get(), attribute_type,
                             parent_attributes.get()[attribute_type], NULL);
      }

      if (result != 0) {
        fprintf(stderr, "failed to parse nested attributes!\n");
        return NL_SKIP;
      }

      parent_attributes = std::move(nested_attributes);
      if (i > 1) {
        continue;
      }
      // Get the attribute value. After last iteration, the value is true for
      // nested attributes.
      attribute_to_read_type = static_cast<int>(attribute_to_read->type);
      if ((attribute_to_read_type <= attribute_type) &&
          (parent_attributes.get()[attribute_to_read_type])) {
        attribute_to_read_value =
            nla_data(parent_attributes[attribute_to_read_type]);
      }
      std::cout << "Iteration " << i << "\n";
    }

    switch (attribute_to_read->value_type) {
      case Attribute::ValueTypes::UINT32:
        if (!attribute_to_read->value) {
          break;
        }
        *static_cast<uint32_t *>(attribute_to_read->value) =
            *static_cast<uint32_t *>(attribute_to_read_value);
        break;

      case Attribute::ValueTypes::UINT48:
        if (!attribute_to_read->value) {
          break;
        }
        char tmp_str[18];
        sprintf(tmp_str,
                "%02x:%02x:%02x:%02x:%02x:%02x",
                *(static_cast<const char *>(attribute_to_read_value)) & 0xff,
                *(static_cast<const char *>(attribute_to_read_value) + 1) & 0xff,
                *(static_cast<const char *>(attribute_to_read_value) + 2) & 0xff,
                *(static_cast<const char *>(attribute_to_read_value) + 3) & 0xff,
                *(static_cast<const char *>(attribute_to_read_value) + 4) & 0xff,
                *(static_cast<const char *>(attribute_to_read_value) + 5) & 0xff);
        static_cast<std::string *>(attribute_to_read->value)->assign(tmp_str);
        break;

      case Attribute::ValueTypes::STRING:
        if (!attribute_to_read->value) {
          break;
        }
        static_cast<std::string *>(attribute_to_read->value)->assign(
                static_cast<const char *>(attribute_to_read_value));
        break;

        // case Attribute::ValueTypes::NESTED:
        //   if (!attribute_to_read->value) {
        //     break;
        //   }

        //   if (!static_cast<NestedAttr *>(attribute_to_read->value)->attr ||
        //       !static_cast<NestedAttr *>(attribute_to_read->value)->policy) {
        //     fprintf(stderr, "no policy/attr!\n");
        //     break;
        //   }

        //   if (!attributes.get()[NL80211_ATTR_BSS]) {
        //     fprintf(stderr, "bss info missing!\n");
        //     break;
        //   }

        //   for (uint8_t i = 0; i < NL80211_BSS_MAX + 1; ++i) {
        //     fprintf(stderr, "i %d!\n", i);
        //     std::cout << " and "
        //               << static_cast<NestedAttr
        //               *>(attribute_to_read->value)->policy[i].type
        //               << "\n";
        //     if (static_cast<NestedAttr
        //     *>(attribute_to_read->value)->policy[i].type >
        //         NLA_TYPE_MAX)
        //       fprintf(stderr, "Too big!\n");
        //   }

        //   if (nla_parse_nested(static_cast<NestedAttr
        //   *>(attribute_to_read->value)->attr,
        //           NL80211_BSS_MAX,
        //           attributes.get()[NL80211_ATTR_BSS],
        //           static_cast<NestedAttr
        //           *>(attribute_to_read->value)->policy)) {
        //     fprintf(stderr, "failed to parse nested attributes!\n");
        //     break;
        //   }

        //   if (!(static_cast<NestedAttr *>(attribute_to_read->value))
        //             ->attr[NL80211_BSS_BSSID]) {
        //     break;
        //   }
        //   if (!(static_cast<NestedAttr *>(attribute_to_read->value))
        //             ->attr[NL80211_BSS_STATUS]) {
        //     break;
        //   }

        //   // mac_addr_n2a(mac_addr, nla_data(bss[NL80211_BSS_BSSID]));
        //   char dev[20];
        //   if_indextoname(nla_get_u32(attributes.get()[NL80211_ATTR_IFINDEX]),
        //   dev); std::cout << "Interface " << dev << "\n";

        //   for (uint8_t i = 0; i < NL80211_BSS_MAX + 1; ++i) {
        //     struct nlattr *attri =
        //         static_cast<NestedAttr *>(attribute_to_read->value)->attr[i];
        //     if (!attri) {
        //       continue;
        //     }

        //     void *nested_attribute_value = nla_data(attri);
        //     std::cout << "Value: " << *(uint32_t *)nested_attribute_value
        //               << "\n";
        //   }

        //             // switch (nla_get_u32(bss[NL80211_BSS_STATUS])) {
        //             // case NL80211_BSS_STATUS_ASSOCIATED:
        //             //   printf("Connected to %s (on %s)\n", mac_addr, dev);
        //             //   break;
        //             // case NL80211_BSS_STATUS_AUTHENTICATED:
        //             //   printf("Authenticated with %s (on %s)\n", mac_addr,
        //             dev);
        //             //   return NL_SKIP;
        //             // case NL80211_BSS_STATUS_IBSS_JOINED:
        //             //   printf("Joined IBSS %s (on %s)\n", mac_addr, dev);
        //             //   break;
        //             // default:
        //             //   return NL_SKIP;

      default:;
    }
  }
  return NL_OK;
}

void Communicator::challenge(const Nl80211Commands &command,
                             const Message::Flags &flags,
                             const std::vector<const Attribute *> *attr_arg,
                             const std::vector<Attribute *> *attr_read) {
  auto socket  = std::make_unique<Socket>(socket_cb_kind_);
  auto message = std::make_unique<Message>();

  // Add Netlink header, Generic Netlink header to the message.
  if (!genlmsg_put(message->get_message(),   // message
                   NL_AUTO_PORT,             // port (auto)
                   NL_AUTO_SEQ,              // sequence (auto)
                   nl80211_family_id_,       // family (nl80211) id
                   0,                        // user header len
                   static_cast<int>(flags),  // message flags
                   command,                  // command
                   0))                       // interface version
  {
    throw Exception("Communicator:challenge:message header adding failed");
  }

  add_attributes(message->get_message(), attr_arg);

  send_and_receive(socket->get_socket(), message->get_message(), attr_read);
}

void Communicator::set_callback_kind(const CallbackKind &kind) {
  socket_cb_kind_ = kind;
}

const std::string &Communicator::get_error_report() const {
  return error_report_;
}

Communicator::~Communicator() { nl_cb_put(callback_); }

// Message handlers.

int error_handler(LibnlSocketAddress *nla,
                  LibnlErrorMessageHeader *err_msg,
                  std::string *arg) {
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

  arg->append("Message type: " + std::to_string(err_msg->msg.nlmsg_type) + ";");
  arg->append("Error code: " + Exception::err_to_str(err_msg->error) + ";\n");

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
  nla_parse(err_attributes,
            NLMSGERR_ATTR_MAX,
            attributes,
            (header->nlmsg_len - ack_length),
            NULL);
  // Try to get specific error attribute.
  if (err_attributes[NLMSGERR_ATTR_MSG]) {
    arg->append(
        static_cast<char *>(nla_data(err_attributes[NLMSGERR_ATTR_MSG])));
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