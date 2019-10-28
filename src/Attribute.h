#ifndef __mynl_Attribute__
#define __mynl_Attribute__

#include <stdint.h>
#include <linux/nl80211.h>
#include <unordered_map>
#include <set>
#include <string>

typedef enum nl80211_commands                   nl80211_command_t;
typedef enum nl80211_attrs                      nl80211_attr_type_t;
typedef struct nlattr                           nlattr_t;

namespace mynl
{

// class that is parent class for every attribute class.
class Attribute
{
public:
	enum class Command
	{
		Set,
		Get,
		New,
		Del
	};

	enum class AttributeValueType
	{
		UINT32,
		STRING
	};

	typedef struct attr_block
	{
		void                          * attr_class_member;
		nl80211_attr_type_t             attr_type;
		Attribute::AttributeValueType   attr_val_type;

		bool operator<(const struct attr_block &t) const
		{
			return ((int)this->attr_type < (int)t.attr_type);
		}
	} attr_block_t;

protected:
	attr_block_t identifier;
	nl80211_attr_type_t attribute;
	void * attribute_value;

protected:
	std::unordered_map<Command, nl80211_command_t>      command_map;
	std::unordered_map<Command, std::set<attr_block_t>> attribute_map;

public:
	virtual attr_block_t * get_identifier(void ** arg) = 0;

public:
	nl80211_command_t resolve_command(Command cmd);
	std::set<Attribute::attr_block_t> * resolve_attribute_set(Command cmd);
};

// TODO Attribute classes.

class Wiphy
	: public Attribute
{
public:
	Wiphy(void);
	Wiphy(std::string name);

public:
	virtual attr_block_t * get_identifier(void ** arg);

public:
	uint32_t    id;
	std::string name;
	std::string	addr;

};

class Interface
	: private Attribute
{ // TODO
public:
	uint32_t index;
	char *name;
};

} // namespace mynl
#endif // defined __mynl_Attribute__