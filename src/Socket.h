
#ifndef __mynl_Socket__
#define __mynl_Socket__

#include "mynl.h"

typedef struct nl_sock nl_sock_t;
typedef struct nl_cb   nl_cb_t;

namespace mynl {

class Socket {
private:
    nl_sock_t * socket;
    nl_cb_t   * callback;
    int         nl80211_family_id;
    bool        valid;

public:
    Socket(void);

public:
    mynlret_t   set_callback(nl_cb_t * cb);
    int         get_family_id(void);
    nl_sock_t * get_socket();

}; // Socket

} // namespace mynl

#endif // defined __mynl_Socket__