
#include "Socket.h"

#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/socket.h>

#include "Exception.h"

namespace wiphynlcontrol {

Socket::Socket() {
  socket_ = nl_socket_alloc();
  if (!socket_) {
    throw Exception("Socket(): socket allocation failed");
  }

  // Set socket fd option NETLINK_EXT_ACK
  int option_value = 1;
  if ((setsockopt(nl_socket_get_fd(socket_), SOL_NETLINK, NETLINK_EXT_ACK,
                 (void *)&option_value, sizeof(option_value))) < 0) {
    throw Exception("Socket(): setsockopt() failed");
  }

  callback_ = nl_cb_alloc(NL_CB_DEBUG);
  if (!callback_) {
    throw Exception("Socket(): callback allocation failed");
  }

  if (genl_connect(socket_) < 0) {
    throw Exception("Socket(): genl_connect() exited with non-zero code");
  }

  if ((nl80211_family_id_ = genl_ctrl_resolve(socket_, "nl80211")) < 0) {
    throw Exception(
        "Socket(): genl_ctrl_resolve() exited with negative error code");
  }
}

void Socket::set_callback(LibnlCallback *cb) {
  if (!cb) {
    throw Exception("Socket::set_callback(): argument is NULL");
  }

  callback_ = cb;

  if (!socket_) {
    throw Exception("Socket::set_callback(): socket not allocated");
  }

  nl_socket_set_cb(socket_, callback_);
}

int Socket::get_family_id() { return nl80211_family_id_; }

LibnlSocket *Socket::get_socket() { return socket_; }

}  // namespace wiphynlcontrol