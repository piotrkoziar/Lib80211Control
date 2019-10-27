
#ifndef __mynl_Message__
#define __mynl_Message__

#include "mynl.h"
#include "Attribute.h"
#include "Socket.h"

typedef struct genlmsghdr       genlmsghdr_t;
typedef enum   nl80211_commands nl80211_cmd_t;
typedef struct nl_msg  		    nl_msg_t;

namespace mynl {

class Message {
private:
	enum class AttributeValueType {UINT32, STRING};
private:
	nl_msg_t 	   * message;
	int 	 	     message_flags;

	nl80211_cmd_t     	   command;
	nl80211_attr_type_t    attribute_type;
	void 		   	    ** attribute_value;
	AttributeValueType     attribute_value_type;
	bool 		 	       valid;
public:
	Message();
	Message(Attribute * attr, Attribute::Command cmd, void ** arg);

private:
	/* TODO This function gets the Attribute and Command (Command states what to do with the attribute, i.e. set, get)
	* and finds corresponding (Message::command) and (Message::attribute_type) and (Message::attribute_value_type).
	*/
	void command_attribute_resolve(Attribute * attr, Attribute::Command cmd);
	void add_attribute(void);

public:
	/* Method to get attributes from the message. */
	mynlret_t get_attr(Attribute * attr);

	/* Call this method to prepare the message. Must be called before the send() method. */
	void prepare_message(Attribute * attr, Attribute::Command cmd, void ** arg);

	/* Send method. */
	mynlret_t send(Socket * socket);
}; // class NetlinkMessage

} // namespace mynl

#endif // defined __mynl_Message__