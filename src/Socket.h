
#ifndef WIPHYNLCONTROL_SOCKET_H_
#define WIPHYNLCONTROL_SOCKET_H_

#include <netlink/handlers.h>

typedef struct nl_sock  LibnlSocket;
typedef struct nl_cb    LibnlCallback;
typedef enum nl_cb_kind LibnlCallbackKind;

namespace wiphynlcontrol {

class Socket {
 private:
  LibnlSocket   *socket_;
  LibnlCallback *callback_;
  int           nl80211_family_id_;

 public:
  Socket(LibnlCallbackKind cb_kind = NL_CB_DEBUG);

 public:
  // Sets Socket callback.
  void set_callback(const LibnlCallback *cb);

  // Returns the numeric identifier for the Socket family name.
  // Socket family name is Generic Netlink and it is set in the constructor.
  int get_family_id() const;

  // Returns pointer to the LibnlSocket member.
  LibnlSocket *get_socket() const;

};  // Socket

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_SOCKET_H_