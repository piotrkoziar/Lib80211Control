
#ifndef WIPHYNLCONTROL_SOCKET_H_
#define WIPHYNLCONTROL_SOCKET_H_

#include <netlink/handlers.h>

typedef struct nl_sock  LibnlSocket;
typedef struct nl_cb    LibnlCallback;
typedef enum nl_cb_kind LibnlCallbackKind;

typedef enum {
  CALLBACK_DEFAULT = LibnlCallbackKind::NL_CB_DEFAULT,
  CALLBACK_DEBUG   = LibnlCallbackKind::NL_CB_DEBUG
} CallbackKind;

namespace wiphynlcontrol {

class Socket {
 private:
  LibnlSocket   *socket_;
  LibnlCallback *callback_;

 public:
  Socket(CallbackKind cb_kind = CALLBACK_DEFAULT);

 public:
  // Sets Socket callback.
  void set_callback(const LibnlCallback *cb);

  // Returns pointer to the LibnlSocket member.
  LibnlSocket *get_socket() const;

 public:
  ~Socket();
};  // Socket

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_SOCKET_H_