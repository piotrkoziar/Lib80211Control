#include <net/if.h>
#include <netlink/socket.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <linux/nl80211.h>

typedef struct nl_sock 		  nl_sock_t;
typedef struct nl_msg  		  nl_msg_t;
typedef struct nl_cb   		  nl_cb_t;
typedef enum nl80211_commands nl80211_cmd_t;
typedef enum nl80211_attrs	  nl80211_attr_t;
typedef struct genlmsghdr	  genlmsghdr_t;
typedef struct nlattr 		  nlattr_t;
