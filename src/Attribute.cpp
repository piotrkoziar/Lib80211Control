#include "Attribute.h"
#include <iostream>

using namespace mynl;

nl80211_command_t Attribute::resolve_command(Command cmd) // TODO throws exception
{
    nl80211_command_t attribute_type;

    try
    {
        attribute_type = command_map.at(cmd);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return NL80211_CMD_UNSPEC;
    }

    return attribute_type;
}

/* TODO attribute types must be connected with Wiphy members.
 * attribute type values must be connected with attribute types.
 * (more maps)
 */
Wiphy::Wiphy()
{
    command_map =
    {
        {Command::Set, NL80211_CMD_SET_WIPHY},
        {Command::Get, NL80211_CMD_GET_WIPHY},
        {Command::New, NL80211_CMD_NEW_WIPHY},
        {Command::Del, NL80211_CMD_DEL_WIPHY}
    };
}