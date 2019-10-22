
#include "GenericNetlinkMessage.h"

using namespace genericnetlink;


Message::Message(nl_msg_t * msg)
{
	header = (genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	nla_parse(attributes, NL80211_ATTR_MAX, genlmsg_attrdata(header, 0), genlmsg_attrlen(header, 0), NULL);
}

genlret_t Message::get_attr(const nl80211_attr_t attr_name, void ** attr)
{
	if (attributes[attr_name])
	{
		*attr = nla_data(attributes[attr_name]);
		return GENLRET_SUCCESS;
	}

	return GENLRET_ERROR;
}
