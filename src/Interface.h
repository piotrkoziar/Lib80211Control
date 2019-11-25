#ifndef WIPHYNLCONTROL_INTERFACE_H_
#define WIPHYNLCONTROL_INTERFACE_H_

#include <string>
#include "Entity.h"
#include "Property.h"

namespace wiphynlcontrol {

    class Interface : public Entity {
     public:
      explicit Interface(const uint32_t &id);

     public:
      Property<uint32_t> index_;
      Property<std::string> addr_;
      Property<std::string> ssid_;
      Property<uint8_t> type_;
    };
}
#endif