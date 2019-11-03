
#include "Message.h"
#include <iostream>
#include <netlink/netlink.h>
#include <netlink/msg.h>
#include <netlink/genl/genl.h>

using namespace wiphynlcontrol;

Message::Message()
{
	message = nlmsg_alloc();
	if (!message)
	{
		// TODO return value ?
		// Exceptions maybe
	}

	callback = nl_cb_alloc(NL_CB_DEBUG);
	if (!callback)
	{

	}

}

Message::Message(Entity * attr, Entity::Commands cmd, void ** arg)
{
	Message();

	// TODO
	// prepare_message(attr, cmd, arg);
	// cannot do it here: message is not completly constructed yet

}

void Message::command_attribute_resolve(Entity * attr, Entity::Commands cmd)
{
	// TODO method from the Attribute class should be called to resolve command, attribute_type, attribute_value_type
	std::cout << "resolve command\n";
	command    	  = attr->resolve_command(cmd);
	std::cout << "resolve attribute set\n";
	attribute_set = attr->resolve_attribute_set(cmd);
	std::cout << "resolved attribute set\n";

}

void Message::add_attribute(Nl80211AttributeTypes attr_type, Entity::AttributeValueTypes attr_val_type, void * attr_value)
{
	switch (attr_val_type)
	{
		case Entity::AttributeValueTypes::UINT32 :
			NLA_PUT_U32(message, attr_type, *((uint32_t *)attr_value));
			break;

		case Entity::AttributeValueTypes::STRING :
			std::cout << "add_attribute(): member: \t"  << *((std::string *)attr_value) << "\n";
			NLA_PUT_STRING(message, attr_type, (char *)attr_value);
			break;

		default :
		nla_put_failure:
			break;
	}
}

void Message::get_attr()
{
	genlmsghdr * header;
	NlAttribute   * attributes[NL80211_ATTR_MAX + 1];

	if (!(header = (genlmsghdr *)nlmsg_data(nlmsg_hdr(message))))
	{
		return;
	}

	nla_parse(attributes, NL80211_ATTR_MAX, genlmsg_attrdata(header, 0), genlmsg_attrlen(header, 0), NULL);

	void * attribute_value;
	for(auto itr = attribute_set->begin(); itr != attribute_set->end(); ++itr)
	{
		if (attributes[itr->attr_type])
		{
			attribute_value = nla_data(attributes[itr->attr_type]);
			switch (itr->attr_val_type)
			{
				default:
				case Entity::AttributeValueTypes::UINT32 :
					*(uint32_t *)itr->attr_class_member = *(uint32_t *)attribute_value;
					break;

				case Entity::AttributeValueTypes::STRING :
					*(std::string *)itr->attr_class_member = *(std::string *)attribute_value;
			}
		}
	}
}

/* Call this method to prepare the message. Must be called before the send() method. */
void Message::prepare_message(Entity * attr, Entity::Commands cmd, void ** arg)
{
	// TODO : case when no arg

	// Obtain info
	command_attribute_resolve(attr, cmd);

	Entity::AttributeBlock * identifier = attr->get_identifier(arg);

	std::cout << "got: identifier: type \t"  << identifier->attr_type << "\n";
	std::cout << "member: \t"  << *((std::string *)identifier->attr_class_member) << "\n";

	// TODO
	add_attribute(identifier->attr_type, identifier->attr_val_type, identifier->attr_class_member);
	std::cout << "message flags:" << "\n";
	message_flags = NLM_F_DUMP;
	std::cout << message_flags << "\n";
}

mynlret_t Message::send(Socket * socket)
{
	if (!command || !message_flags || !message || !socket)
	{
		return MYNL_ERROR;
	}

	std::cout << "send()\n";

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

	// Send the message

	if (nl_send_auto(socket->get_socket(), message) < 0)
	{
		return MYNL_ERROR;
	}

	// Set up callbacks

	int ret = 1;

	nl_cb_err(callback, NL_CB_CUSTOM, error_handler, (void *)&ret);
	nl_cb_set(callback, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, (void *)&ret);
	nl_cb_set(callback, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, (void *)&ret);
	nl_cb_set(callback, NL_CB_VALID, NL_CB_CUSTOM, valid_handler, (void *)this);

	// Get the

	while (ret > 0)
	{
		nl_recvmsgs(socket->get_socket(), callback);
	}

	return MYNL_SUCCESS;
}

int Message::error_handler(sockaddr_nl_t * nla, nlmsgerr_t * err_msg, void * arg)
{
	/*
	[NETLINK HEADER] 16 octets
		.nlmsg_len = 36
		.type = 2 <ERROR>
		.flags = 256 <ROOT>
		.seq = 1572300661
		.port = -327123043
	[ERRORMSG] 20 octets <---------err_msg
		.error = 0 "Success"
	[ORIGINAL MESSAGE] 16 octets
		.nlmsg_len = 16
		.type = 16 <0x10>
		.flags = 5 <REQUEST,ACK>
		.seq = 1572300661
		.port = -327123043
	*/

	*(int *)arg = err_msg->error;
	if (err_msg->error == 0)
	{
		// Success
		return NL_STOP;
	}

	if (!(err_msg->msg.nlmsg_flags & NLM_F_ACK_TLVS))
	{
		return NL_STOP;
	}

	uint32_t ack_length;
	if (!(err_msg->msg.nlmsg_flags & NLM_F_CAPPED))
	{
		ack_length = sizeof(struct nlmsghdr) + sizeof(int) + err_msg->msg.nlmsg_len;
	}
	else
	{
		ack_length = sizeof(struct nlmsghdr) + sizeof(int) + sizeof(struct nlmsghdr);
	}

	struct nlmsghdr * header = (struct nlmsghdr *)err_msg - 1;
	if(header->nlmsg_len <= ack_length)
	{
		// There are no attributes
		return NL_STOP;
	}

	NlAttribute * attributes = (NlAttribute *)((unsigned char *)header + ack_length);
	NlAttribute * err_attributes[NLMSGERR_ATTR_MAX + 1];

	nla_parse(err_attributes, NLMSGERR_ATTR_MAX, attributes, (header->nlmsg_len - ack_length), NULL);
	if (err_attributes[NLMSGERR_ATTR_MSG])
	{
		// TODO save the attribute value in the exception
		int attr_value_len = strnlen((char *)nla_data(err_attributes[NLMSGERR_ATTR_MSG]), nla_len(err_attributes[NLMSGERR_ATTR_MSG]));
		fprintf(stderr, "kernel reports: %*s\n", attr_value_len, (char *)nla_data(err_attributes[NLMSGERR_ATTR_MSG]));
	}

	return NL_STOP;
}
int Message::ack_handler(nl_msg *msg, void * arg)
{
	*(int *)arg = 0;
	return NL_STOP;
}
int Message::finish_handler(nl_msg_t *msg, void * arg)
{
	*(int *)arg = 0;
	return NL_SKIP;
}
int Message::valid_handler(nl_msg_t *msg, void * arg)
{
	Message * message = (Message *)arg;

	message->get_attr();
	return NL_OK;
}
