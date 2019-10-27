#ifndef __mynl_Attribute__
#define __mynl_Attribute__

#include <stdint.h>
// #include "mynl.h"

typedef enum   nl80211_attrs nl80211_attr_type_t;
typedef struct nlattr 		 nlattr_t;

// class that is parent class for every attribute class.
class Attribute
{
public:
	enum class Command {Set, Get};

public:
	nl80211_attr_type_t attribute;
};

// TODO Attribute classes.

class Wiphy
	: private Attribute
{ // TODO
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


#endif // defined __mynl_Attribute__