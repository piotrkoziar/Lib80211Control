#ifndef __Wiphy__
#define __Wiphy__

#include <stdint.h>

class Wiphy {
public:
	uint32_t   id;
	char     * name;
};

class Interface {
public:
	uint32_t   index;
	char 	 * name;
};


#endif // defined __Wiphy__