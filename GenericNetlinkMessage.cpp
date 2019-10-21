
#include "GenericNetlinkMessage.h"

using namespace genericnetlink;


Message::Message(nl_msg_t * msg)
{
	header = (genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	nla_parse(attributes, NL80211_ATTR_MAX, genlmsg_attrdata(header, 0), genlmsg_attrlen(header, 0), NULL);
}

Wiphy Message::get_attr_wiphy()
{
	Wiphy wiphy;

	if (attributes[NL80211_ATTR_WIPHY])
	{
		wiphy.id = nla_get_u32(attributes[NL80211_ATTR_WIPHY]);
	}
	else
	{
		return wiphy;
	}

	if (attributes[NL80211_ATTR_WIPHY_NAME])
	{
		wiphy.name = nla_get_string(attributes[NL80211_ATTR_WIPHY_NAME]);
	}

	return wiphy;
}
