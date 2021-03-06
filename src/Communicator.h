
#ifndef WIPHYNLCONTROL_COMMUNICATOR_H_
#define WIPHYNLCONTROL_COMMUNICATOR_H_

#include <vector>

#include "Message.h"
#include "Property.h"
#include "Socket.h"

namespace wiphynlcontrol {

class Communicator {
 private:
  enum class ScanTriggerResult {UNSPECIFIED, DONE, ABORTED};
 private:
  int nl80211_family_id_;
  int scan_multicast_group_id_;
  CallbackKind socket_cb_kind_;
  LibnlCallback *callback_;
  std::string error_report_;

 public:
  explicit Communicator(const CallbackKind &cb_kind = CALLBACK_DEFAULT);

 private:
  // Adds attribute to the message. Use before requesting the kernel.
  void add_attributes(LibnlMessage *message,
                      const std::vector<const Attribute *> *attr_arg);
  // Uses the socket to query the kernel for numeric identifier of the
  // Generic Netlink family name. Sets nl80211_family_id_ with the result.
  void set_family_id(LibnlSocket *socket);
  void set_callback_default();
  // Sends the message_ and gets the answer.
  void send_and_receive(LibnlSocket *socket,
                        LibnlMessage *message);
  // Gets attributes from the message. Use as callback.
  static int get_attributes(LibnlMessage *msg,
                            const std::vector<Attribute *> *attr_read);
  static int get_scan_trigger_result(LibnlMessage *msg, ScanTriggerResult *arg);

 public:
  // Prepares message and socket. Uses given command and
  // attributes to request and get the answer from the kernel.
  // @param attr_arg - attribute attached to the request. Usually the
  // identifier.
  // @param attr_read - attributes expected to be present in kernel's response.
  void challenge(const Nl80211Commands &command,
                 const Message::Flags &flags,
                 const std::vector<const Attribute *> *attr_arg,
                 const std::vector<Attribute *> *attr_read);
  void trigger_scan(int iface_index);
  void set_callback_kind(const CallbackKind &kind);
  const std::string &get_error_report() const;

 public:
  ~Communicator();
};  // class Communicator

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_COMMUNICATOR_H_