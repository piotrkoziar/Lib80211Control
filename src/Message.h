
#ifndef __mynl_Message__
#define __mynl_Message__

#include "mynl.h"
#include "Attribute.h"
#include "Socket.h"

typedef struct genlmsghdr       genlmsghdr_t;
typedef enum   nl80211_commands nl80211_cmd_t;
typedef struct nl_msg  		    nl_msg_t;
typedef struct sockaddr_nl		sockaddr_nl_t;
typedef struct nlmsgerr			nlmsgerr_t;

typedef std::set<mynl::Attribute::attr_block_t> attr_set_t;

namespace mynl {

class Message {
private:

private:
	nl_msg_t 	  * message;
	int 	 	    message_flags;
	nl80211_cmd_t   command;
	attr_set_t	  * attribute_set;
	bool 		    valid;
	nl_cb_t		  * callback;

public:
	Message();
	Message(Attribute * attr, Attribute::Command cmd, void ** arg);

private:
	/* TODO This function gets the Attribute and Command (Command states what to do with the attribute, i.e. set, get)
	* and finds corresponding (Message::command) and (Message::attribute_type) and (Message::attribute_value_type).
	*/
	void command_attribute_resolve(Attribute * attr, Attribute::Command cmd);
	void add_attribute(nl80211_attr_type_t attr_type, Attribute::AttributeValueType attr_val_type, void ** attr_value);

/* callbacks */
private:
	static int error_handler(sockaddr_nl_t * nla, nlmsgerr_t * err, void * arg);
    static int ack_handler(nl_msg *msg, void * arg);
	static int finish_handler(nl_msg_t *msg, void * arg);
	static int valid_handler(nl_msg_t *msg, void * arg);

public:
	/* Method to get attributes from the message. */
	void get_attr();

	/* Call this method to prepare the message. Must be called before the send() method. */
	void prepare_message(Attribute * attr, Attribute::Command cmd, void ** arg);

	/* Send method. */
	mynlret_t send(Socket * socket);
}; // class NetlinkMessage

} // namespace mynl

#endif // defined __mynl_Message__