#ifndef WIPHYNLCONTROL_ATTRIBUTE_H_
#define WIPHYNLCONTROL_ATTRIBUTE_H_

#include <linux/nl80211.h>
#include <stdint.h>
#include <set>
#include <string>
#include <unordered_map>

typedef enum nl80211_commands Nl80211Commands;
typedef enum nl80211_attrs    Nl80211AttributeTypes;
typedef struct nlattr         NlAttribute;

namespace wiphynlcontrol {

// This is parent class for the Wiphy, Interface etc. classes.
class Entity {
 public:
  enum class Commands { Set, Get, New, Del };

  enum class AttributeValueTypes { UINT32, STRING };

  typedef struct AttributeBlock {
    void                  *attr_class_member;
    Nl80211AttributeTypes attr_type;
    AttributeValueTypes   attr_val_type;
    // Operator needed for the std::set at() call.
    bool operator<(const struct AttributeBlock &t) const {
      return (static_cast<int>(this->attr_type) <
              static_cast<int>(t.attr_type));
    }
  } AttributeBlock;

 protected:
  AttributeBlock        identifier_;
  Nl80211AttributeTypes attribute_;
  void                  *attribute_value_;

 protected:
  // Maps command to the corresponding nl80211 command. TODO also identifier
  std::unordered_map<Commands, Nl80211Commands> command_map_;
  // Maps command to the set of the attributes that are connected
  // with the command
  std::unordered_map<Commands, std::set<AttributeBlock>> attribute_map_;

 public:
  // TODO identifier thing should be replaced with specifying the necessary
  // attribute in the command map.
  virtual AttributeBlock *get_identifier(void **arg) = 0;

 public:
  // Returns matching nl80211 command. Uses command_map_ member. TODO
  Nl80211Commands resolve_command(Commands cmd);
  // Returns pointer to the corresponding std::set. Uses attribute_map_ member.
  std::set<Entity::AttributeBlock> *resolve_attribute_set(Commands cmd);
};

// Here declare classes derived from Entity class.

class Wiphy : public Entity {
 public:
  Wiphy();
  Wiphy(std::string name);

 public:
  uint32_t    id_;
  std::string name_;
  std::string address_;
  uint32_t    frequency_;

 private:
  // Fills command_map_ and attribute_map_.
  // Here apply changes related to command to attribute mapping.
  void setup_maps();

 public:
  // TODO
  virtual AttributeBlock *get_identifier(void **arg);
};

class Interface : public Entity {  // TODO
 public:
  uint32_t    index_;
  std::string *name_;
};

}  // namespace wiphynlcontrol
#endif  // defined WIPHYNLCONTROL_ATTRIBUTE_H_