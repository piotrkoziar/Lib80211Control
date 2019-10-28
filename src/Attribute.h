#ifndef __mynl_Attribute__
#define __mynl_Attribute__

#include <stdint.h>
#include <linux/nl80211.h>
#include <unordered_map>

typedef enum   nl80211_commands nl80211_command_t;
typedef enum   nl80211_attrs    nl80211_attr_type_t;
typedef struct nlattr 		    nlattr_t;

namespace mynl
{

// class that is parent class for every attribute class.
class Attribute
{
public:
	enum class Command {Set, Get, New, Del};

protected:
	std::unordered_map <Command, nl80211_command_t> command_map;

protected:
	nl80211_command_t resolve_command(Command cmd);

};

// TODO Attribute classes.

class Wiphy
	: public Attribute
{
public:
	Wiphy(void);
public:
	uint32_t   id;
	char     * name;
};

class Interface
	: private Attribute
{ // TODO
public:
	uint32_t   index;
	char 	 * name;
};

} // namespace mynl
#endif // defined __mynl_Attribute__