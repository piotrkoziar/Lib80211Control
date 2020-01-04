
#include "Socket.h"

#include <netlink/genl/genl.h>
#include <netlink/socket.h>

#include <cassert>
#include <iostream>

#include "Exception.h"

namespace wiphynlcontrol {

Socket::Socket(const CallbackKind cb_kind) {
  socket_ = nl_socket_alloc();
  if (!socket_) {
    throw Exception("Socket:Socket:socket allocation failed");
  }

  if (genl_connect(socket_) < 0) {
    throw Exception("Socket:Socket:genl_connect:exited with non-zero code");
  }

  // Set socket fd option NETLINK_EXT_ACK
  int option_value = 1;
  if (setsockopt(nl_socket_get_fd(socket_),
                  SOL_NETLINK,
                  NETLINK_EXT_ACK,
                  &option_value,
                  sizeof(option_value)) < 0) {
    throw Exception(append_errno_to_str(
        "Socket:Socket:setsockopt:setting socket option failed : "));
  }

  if (nl_socket_set_nonblocking(socket_) < 0) {
    throw Exception(append_errno_to_str(
        "Socket:Socket:setsockopt:setting socket nonblocking failed : "));
  }

  callback_ = nl_cb_alloc(static_cast<LibnlCallbackKind>(cb_kind));
  if (!callback_) {
    throw Exception("Socket:Socket:callback allocation failed");
  }
  nl_socket_set_cb(socket_, callback_);
}

void Socket::set_callback(const LibnlCallback *cb) {
  if (!cb) {
    throw Exception("Socket:set_callback:argument is null");
  }
  callback_ = const_cast<LibnlCallback *>(cb);

  if (!socket_) {
    throw Exception("Socket:set_callback:socket not allocated");
  }

  nl_socket_set_cb(socket_, callback_);
}

void Socket::add_membership(const int &group_id) {
  nl_socket_add_membership(socket_, group_id);
}

LibnlSocket *Socket::get_socket() const { return socket_; }

Socket::~Socket() {
  nl_cb_put(callback_);
  nl_socket_free(socket_);
}

}  // namespace wiphynlcontrol