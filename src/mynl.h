#ifndef __mynl__
#define __mynl__

// TODO Headers! Cleanup
// #include <net/if.h>
// #include <netlink/socket.h>
// #include <netlink/netlink.h>
// #include <netlink/genl/genl.h>
// #include <netlink/genl/ctrl.h>
// #include <netlink/msg.h>
// #include <netlink/attr.h>
#include <linux/nl80211.h>

namespace mynl {

typedef enum {
    MYNL_SUCCESS = 0,
    MYNL_ERROR   = -1,
} mynlret_t;

} // namespace mynl

#endif // declared __mynl__