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

std::set<Attribute::attr_block_t> * Attribute::resolve_attribute_set(Command cmd) // TODO throws exception
{
    try
    {
        return &attribute_map.at(cmd);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return NULL;
    }
}

/* TODO attribute types must be connected with Wiphy members.
 * attribute type values must be connected with attribute types.
 * (more maps)
 */
Wiphy::Wiphy()
{
    command_map =
    {
        {
            Command::Set, NL80211_CMD_SET_WIPHY
        },
        {
            Command::Get, NL80211_CMD_GET_WIPHY
        },
        {
            Command::New, NL80211_CMD_NEW_WIPHY
        },
        {
            Command::Del, NL80211_CMD_DEL_WIPHY
        }
    };

    attribute_map =
    {
        {
            Command::Set,
            std::set<attr_block_t>
            {
                {
                    (void *)&id,
                    NL80211_ATTR_WIPHY,
                    AttributeValueType::UINT32
                },
                {
                    (void *)&name,
                    NL80211_ATTR_WIPHY_NAME,
                    AttributeValueType::STRING
                },
            }
        },
        {
            Command::Get,
            std::set<attr_block_t>
            {
                {
                    (void *)&id,
                    NL80211_ATTR_WIPHY,
                    AttributeValueType::UINT32
                },
                {
                    (void *)&name,
                    NL80211_ATTR_WIPHY_NAME,
                    AttributeValueType::STRING
                },
            }
        },
        {
            Command::New,
            std::set<attr_block_t>
            {
                {
                    (void *)&id,
                    NL80211_ATTR_WIPHY,
                    AttributeValueType::UINT32
                },
                {
                    (void *)&name,
                    NL80211_ATTR_WIPHY_NAME,
                    AttributeValueType::STRING
                },
            }
        },
        {
            Command::Del,
            std::set<attr_block_t>
            {
                {
                    (void *)&id,
                    NL80211_ATTR_WIPHY,
                    AttributeValueType::UINT32
                },
                {
                    (void *)&name,
                    NL80211_ATTR_WIPHY_NAME,
                    AttributeValueType::STRING
                },
            }
        }
    };


    // TODO maybe find in the attribute_map what are the values for the name class member
    identifier =
    {
        .attr_class_member = (void *)&name,
        .attr_type         = NL80211_ATTR_WIPHY_NAME,
        .attr_val_type     = AttributeValueType::STRING,
    };
}

Wiphy::Wiphy(std::string nm) : name(nm)
{
    Wiphy();
}

Attribute::attr_block_t * Wiphy::get_identifier(void ** arg)
{
    if ((*(std::string *)identifier.attr_class_member) == "")
    {
        return NULL;
    }

    return &identifier;
}