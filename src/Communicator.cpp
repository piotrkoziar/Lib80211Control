#include "Communicator.h"

#include <net/if.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/netlink.h>

#include <iostream>
#include <memory>

#include "Exception.h"

typedef struct nlmsghdr LibnlMessageHeader;
typedef struct nlmsgerr LibnlErrorMessageHeader;
typedef struct genlmsghdr LibnlGeMessageHeader;
typedef struct sockaddr_nl LibnlSocketAddress;

static void mac_address_to_string(std::string &str, const void *address) {
  char tmp_str[18];
  sprintf(tmp_str,
          "%02x:%02x:%02x:%02x:%02x:%02x",
          *(static_cast<const char *>(address)) & 0xff,
          *(static_cast<const char *>(address) + 1) & 0xff,
          *(static_cast<const char *>(address) + 2) & 0xff,
          *(static_cast<const char *>(address) + 3) & 0xff,
          *(static_cast<const char *>(address) + 4) & 0xff,
          *(static_cast<const char *>(address) + 5) & 0xff);
  str.assign(tmp_str);
}

static void get_ssid_information_elements(unsigned char *info, uint8_t info_len,
                                          std::string &arg) {
  uint8_t len;
  uint8_t *data;
  int i;
  char buffer[10];
  while (info_len >= 2 && info_len >= info[1]) {
    if (info[0] == 0 && info[1] <= 32) {
      len  = info[1];
      data = info + 2;
      for (i = 0; i < len; i++) {
        if (isprint(data[i]) && data[i] != ' ' && data[i] != '\\') {
          sprintf(buffer, "%c", data[i]);
          arg.append(buffer);
        } else if (data[i] == ' ' && (i != 0 && i != len - 1)) {
          sprintf(buffer, " ");
          arg.append(buffer);
        } else {
          sprintf(buffer, "\\x%.2x", data[i]);
          arg.append(buffer);
        }
      }
      break;
    }
    info_len -= info[1] + 2;
    info += info[1] + 2;
  }
}

namespace wiphynlcontrol {

// Gets the error attribute from the message.
// Assigns character string to the arg parameter.
static int error_handler(LibnlSocketAddress *nla,
                         LibnlErrorMessageHeader *err,
                         std::string *arg);
static int ack_handler(LibnlMessage *msg, void *arg);
static int finish_handler(LibnlMessage *msg, void *arg);
static int seq_check(struct nl_msg *msg, void *arg);

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

  scan_multicast_group_id_ = genl_ctrl_resolve_grp(socket, "nl80211", "scan");
}

void Communicator::set_callback_default() {
  int ret = 1;
  nl_cb_err(callback_, NL_CB_CUSTOM,
            reinterpret_cast<nl_recvmsg_err_cb_t>(error_handler),
            static_cast<void *>(&error_report_));
  nl_cb_set(callback_, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &ret);
  nl_cb_set(callback_, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &ret);
}

void Communicator::send_and_receive(LibnlSocket *socket,
                                    LibnlMessage *message) {
  if (!socket || !message) {
    throw Exception("Communicator:send_and_receive:argument is NULL");
  }

  set_callback_default();

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
  bool present = false;
  int attribute_to_read_type;
  int attribute_type;  // Type of attribute cast to int.
  int result;
  std::unique_ptr<LibnlAttribute *[]> parent_attributes, nested_attributes;
  static auto attributes =
      std::make_unique<LibnlAttribute *[]>(NL80211_ATTR_MAX + 1);
  LibnlAttribute **attributes_to_read_from = attributes.get();

#ifdef COM_DEBUG
  std::cout << "Received ATTRs:\n";
  for (auto type = NL80211_ATTR_UNSPEC; type < NL80211_ATTR_MAX;
       type      = static_cast<Nl80211AttributeTypes>(type + 1)) {
    attribute_to_read_value = nla_data(attributes.get()[type]);
    if (reinterpret_cast<long>(attribute_to_read_value) != NLA_HDRLEN) {
      std::cout << int(type) << "\t" << attribute_to_read_value << std::endl;
    }
  }
#endif

  if (!attr_read) {
    return NL_OK;  // No attributes was specified to read from message.
  }
#ifdef COM_DEBUG
  std::cout << "Requested ATTRs: " << attr_read->size() << std::endl;
  for (auto &it : *attr_read) {
    std::cout << static_cast<int>(it->type) << std::endl;
  }
#endif

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
    attr_vec.clear();

    // Push all parents (from bottom to top) to the vector.
    attr_top = attribute_to_read;
    while (attr_top->parent) {
      attr_top = attr_top->parent;
      attr_vec.push_back(attr_top);
    }
    // Get the attribute value. The value is true only for not-nested
    // attributes.
    attribute_to_read_type = static_cast<int>(attribute_to_read->type);
    if ((attribute_to_read_type <= NL80211_ATTR_MAX) &&
        (attributes.get()[attribute_to_read_type])) {
      attribute_to_read_value =
          nla_data(attributes.get()[attribute_to_read_type]);
      present = true;
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
            nla_data(parent_attributes.get()[attribute_to_read_type]);
        attributes_to_read_from = parent_attributes.get();
        present = true;
      }
    }
    if (!present) {
      return NL_SKIP;
    }
    BSSInfo info = {};
    switch (attribute_to_read->value_type) {
      case Attribute::ValueTypes::INT8:
        if (!attribute_to_read->value) {
          break;
        }
        *static_cast<int8_t *>(attribute_to_read->value) =
                *static_cast<int8_t *>(attribute_to_read_value);
        break;
      case Attribute::ValueTypes::UINT16:
        if (!attribute_to_read->value) {
          break;
        }
        *static_cast<uint16_t *>(attribute_to_read->value) =
            *static_cast<uint16_t *>(attribute_to_read_value);
        break;
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
        mac_address_to_string(
            *static_cast<std::string *>(attribute_to_read->value),
            attribute_to_read_value);
        break;
      case Attribute::ValueTypes::UINT64:
        if (!attribute_to_read->value) {
          break;
        }
        *static_cast<uint64_t *>(attribute_to_read->value) =
            *static_cast<uint64_t *>(attribute_to_read_value);
        break;
      case Attribute::ValueTypes::STRING:
        if (!attribute_to_read->value) {
          break;
        }
        static_cast<std::string *>(attribute_to_read->value)->assign(
                static_cast<const char *>(attribute_to_read_value));
        break;
      case Attribute::ValueTypes::FLAG:
        if (!attribute_to_read->value) {
          break;
        }
        *static_cast<bool *>(attribute_to_read->value) =
            *static_cast<bool *>(attribute_to_read_value);
        break;
      case Attribute::ValueTypes::SCAN:
        if (!attribute_to_read->value) {
          break;
        }
        LibnlAttribute *nested[NL80211_BSS_MAX + 1];
        if (nla_parse_nested(nested, NL80211_BSS_MAX,
                             attributes_to_read_from[attribute_to_read_type],
                             NULL)) {
          break;
        }
        if (nested[NL80211_BSS_BSSID]) {
          info.ssid.assign(static_cast<const char *>(
              nla_data(nested[NL80211_BSS_INFORMATION_ELEMENTS])));
          get_ssid_information_elements(static_cast<unsigned char *>(nla_data(
                                 nested[NL80211_BSS_INFORMATION_ELEMENTS])),
                             nla_len(nested[NL80211_BSS_INFORMATION_ELEMENTS]),
                             info.ssid);
        }
        if (nested[NL80211_BSS_FREQUENCY]) {
          info.frequency = nla_get_u32(nested[NL80211_BSS_FREQUENCY]);
        }
        if (nested[NL80211_BSS_BSSID]) {
          mac_address_to_string(info.bssid,
                                nla_data(nested[NL80211_BSS_BSSID]));
        }
        if (nested[NL80211_BSS_STATUS]) {
		      switch (nla_get_u32(nested[NL80211_BSS_STATUS])) {
            case NL80211_BSS_STATUS_AUTHENTICATED:
              info.status = "authenticated";
              break;
            case NL80211_BSS_STATUS_ASSOCIATED:
              info.status = "associated";
              break;
            case NL80211_BSS_STATUS_IBSS_JOINED:
              info.status = "joined";
              break;
            default:
              info.status = "unknown status: ";
              info.status.append(
                std::to_string(nla_get_u32(nested[NL80211_BSS_STATUS])));
              break;
		      }
    	  }
        static_cast<std::vector<BSSInfo> *>(
          attribute_to_read->value)->push_back(info);
        break;
      case Attribute::ValueTypes::NESTED:
      default:
      break;
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
  nl_cb_set(callback_, NL_CB_VALID, NL_CB_CUSTOM,
            reinterpret_cast<nl_recvmsg_msg_cb_t>(get_attributes),
            const_cast<void *>(static_cast<const void *>(attr_read)));
  send_and_receive(socket->get_socket(), message->get_message());
}

void Communicator::trigger_scan(int iface_index) {
  auto socket  = std::make_unique<Socket>(socket_cb_kind_);
  auto message = std::make_unique<Message>();
  socket->add_membership(scan_multicast_group_id_);
  // Add Netlink header, Generic Netlink header to the message.
  if (!genlmsg_put(message->get_message(),   // message
                   NL_AUTO_PORT,             // port (auto)
                   NL_AUTO_SEQ,              // sequence (auto)
                   nl80211_family_id_,       // family (nl80211) id
                   0,                        // user header len
                   0,                        // message flags
                   NL80211_CMD_TRIGGER_SCAN, // command
                   0))                       // interface version
  {
    throw Exception("Communicator:trigger_scan:message header adding failed");
  }
  // Add attribute to the message.
  nla_put_u32(message->get_message(), NL80211_ATTR_IFINDEX, iface_index);

  ScanTriggerResult result = ScanTriggerResult::UNSPECIFIED;
  nl_cb_set(callback_, NL_CB_VALID, NL_CB_CUSTOM,
            reinterpret_cast<nl_recvmsg_msg_cb_t>(get_scan_trigger_result),
            &result);
  nl_cb_set(callback_, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, seq_check, NULL);
#ifdef COM_DEBUG
  std::cout << "\nTriggering scan...\n";
#endif
  // Error message will be received.
  send_and_receive(socket->get_socket(), message->get_message());
  if (error_report_ != "") {
    throw Exception(
        "Communicator:trigger_scan:received error message is not success:");
    return;
  }
#ifdef COM_DEBUG
  std::cout << "\nWaiting for scan results.\n";
#endif
  // Valid message will be received.
  while (result == ScanTriggerResult::UNSPECIFIED) {
    // Get the answer.
    nl_recvmsgs(socket->get_socket(), callback_);
  }
}

void Communicator::set_callback_kind(const CallbackKind &kind) {
  socket_cb_kind_ = kind;
}

const std::string &Communicator::get_error_report() const {
  return error_report_;
}

int Communicator::get_scan_trigger_result(LibnlMessage *msg,
                                          ScanTriggerResult *arg) {
  LibnlGeMessageHeader *gn_header =
      static_cast<LibnlGeMessageHeader *>(nlmsg_data(nlmsg_hdr(msg)));
  if (gn_header->cmd == NL80211_CMD_NEW_SCAN_RESULTS) {
    *arg = ScanTriggerResult::DONE;
  } else if (gn_header->cmd == NL80211_CMD_SCAN_ABORTED) {
    *arg = ScanTriggerResult::ABORTED;
  }

  return NL_SKIP;
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
int seq_check(struct nl_msg *msg, void *arg)
{
    return NL_OK;
}

}  // namespace wiphynlcontrol