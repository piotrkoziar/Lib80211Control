#ifndef __80211Control_Exception__
#define __80211Control_Exception__

#include <exception>

namespace wiphynlcontrol
{

class Exception : public std::exception {
private:
    const char* info;

public:
    Exception(const char* _info);

public:
    const char * what () const noexcept
    {
    	return this->info;
    }

}; //Exception

} // namspace wiphynlcontrol

#endif // defined __80211Control_Exception__