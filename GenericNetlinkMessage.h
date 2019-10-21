
#ifndef __genericnetlink_Message__
#define __genericnetlink_Message__

#include "mynl.h"
#include "Attributes.h"

namespace genericnetlink {

class Message {
public:
	Message(nl_msg_t * msg);

public:
	genlmsghdr * header;
	nlattr_t   * attributes[NL80211_ATTR_MAX + 1];

public:
	Wiphy get_attr_wiphy(void);

}; // class NetlinkMessage

} // namespace genericnetlink

#endif // defined __genericnetlink_Message__