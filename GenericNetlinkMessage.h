
#ifndef __genericnetlink_Message__
#define __genericnetlink_Message__

#include "mynl.h"

namespace genericnetlink {

class Message {
public:
	Message(nl_msg_t * msg);

public:
	genlmsghdr * header;
	nlattr_t   * attributes[NL80211_ATTR_MAX + 1];

public:
	genlret_t get_attr(const nl80211_attr_t attr_type, void ** attr);

}; // class NetlinkMessage

} // namespace genericnetlink

#endif // defined __genericnetlink_Message__