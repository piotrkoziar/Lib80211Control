
#include "Message.h"
#include <netlink/netlink.h>
#include <netlink/msg.h>
#include <netlink/genl/genl.h>

using namespace mynl;

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

		prepare_message(attr, cmd, arg);

	} while(0);
}

void Message::command_attribute_resolve(Attribute * attr, Attribute::Command cmd)
{
	// TODO method from the Attribute class should be called to resolve command, attribute_type, attribute_value_type
	command    	  = attr->resolve_command(cmd);
	attribute_set = attr->resolve_attribute_set(cmd);
}

void Message::add_attribute(nl80211_attr_type_t attr_type, Attribute::AttributeValueType attr_val_type, void ** attr_value)
{
	switch (attr_val_type)
	{
		case Attribute::AttributeValueType::UINT32 :
			NLA_PUT_U32(message, attr_type, *(uint32_t *)*attr_value);
			break;

		case Attribute::AttributeValueType::STRING :
			NLA_PUT_STRING(message, attr_type, (char *)*attr_value);
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

	// TODO set members in the Attribute class

	// if (attributes[attribute_type])
	// {
	// 	*attribute_value = nla_data(attributes[attribute_type]);
	// 	return MYNL_SUCCESS;
	// }

	return MYNL_ERROR;
}

/* Call this method to prepare the message. Must be called before the send() method. */
void Message::prepare_message(Attribute * attr, Attribute::Command cmd, void ** arg)
{
	// TODO : case when no arg

	// Obtain info
	command_attribute_resolve(attr, cmd);

	Attribute::attr_block_t * identifier = attr->get_identifier(arg);

	// TEMPORARY
	add_attribute(identifier->attr_type, identifier->attr_val_type, &identifier->attr_class_member);
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

	// add_attribute();

	if (nl_send_auto(socket->get_socket(), message) < 0)
	{
		return MYNL_ERROR;
	}

	return MYNL_SUCCESS;
}