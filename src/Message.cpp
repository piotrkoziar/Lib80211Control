
#include "Message.h"
#include <netlink/netlink.h>
#include <netlink/msg.h>
#include <netlink/genl/genl.h>

using namespace mynl;

					// You can pass this values during initialization. If not, see comment to the command_resolve() in Message.h
Message::Message()
{
	message = nlmsg_alloc();
	if (!message)
	{
		// TODO return value ?
		// Exceptions maybe
	}
}

Message::Message(Attribute * attr, Attribute::Command cmd, void ** arg)
{
	do
    {
        message = nlmsg_alloc();
        if (!message)
        {
            break;
            // TODO return value ?
            // Exceptions maybe
        }

		command_attribute_resolve(attr, cmd);

		attribute_value = arg;

	} while(0);
}

void Message::command_attribute_resolve(Attribute * attr, Attribute::Command cmd)
{
	// TODO method from the Attribute class should be called to resolve command, attribute_type, attribute_value_type
}

void Message::add_attribute()
{
	switch (attribute_value_type)
	{
		case AttributeValueType::UINT32 :
			NLA_PUT_U32(message, attribute_type, *(uint32_t *)*attribute_value);
			break;

		case AttributeValueType::STRING :
			NLA_PUT_STRING(message, attribute_type, (char *)*attribute_value);
			break;

		default :
		nla_put_failure:
			break;
	}
}

mynlret_t Message::get_attr(Attribute * attr)
{
	// Obtain info
	command_attribute_resolve(attr, Attribute::Command::Get);

	genlmsghdr * header;
	nlattr_t   * attributes[NL80211_ATTR_MAX + 1];

	if (!(header = (genlmsghdr *)nlmsg_data(nlmsg_hdr(message))))
	{
		return MYNL_ERROR;
	}

	nla_parse(attributes, NL80211_ATTR_MAX, genlmsg_attrdata(header, 0), genlmsg_attrlen(header, 0), NULL);

	if (attributes[attribute_type])
	{
		*attribute_value = nla_data(attributes[attribute_type]);
		return MYNL_SUCCESS;
	}

	return MYNL_ERROR;
}

/* Call this method to prepare the message. Must be called before the send() method. */
void Message::prepare_message(Attribute * attr, Attribute::Command cmd, void ** arg)
{
	// TODO : case when no arg

	// Obtain info
	command_attribute_resolve(attr, cmd);

	attribute_value = arg;
}


mynlret_t Message::send(Socket * socket)
{
	if (!command || !message_flags || !message || !socket)
	{
		return MYNL_ERROR;
	}

	// Add header to the message

	if (!genlmsg_put(message,      	         // message
					 NL_AUTO_PORT,	         // port (auto)
					 NL_AUTO_SEQ,	         // sequence (auto)
					 socket->get_family_id(), // family (nl80211) id
					 0,				         // user header len
					 message_flags,          // message flags
					 command,	 	         // command
					 0))			         // interface version
	{
		return MYNL_ERROR;
	}

	add_attribute();

	if (nl_send_auto(socket->get_socket(), message) < 0)
	{
		return MYNL_ERROR;
	}

	return MYNL_SUCCESS;
}