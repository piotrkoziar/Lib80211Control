
#include "Socket.h"
#include <netlink/socket.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

using namespace mynl;

Socket::Socket()
{
    do
    {
        socket = nl_socket_alloc();
        if (!socket)
        {
            valid = false;
            break;
            // TODO return value ?
            // Exceptions maybe
        }

        int option_value = 1;
        setsockopt(nl_socket_get_fd(socket), SOL_NETLINK, NETLINK_EXT_ACK, (void *)&option_value, sizeof(option_value));

        callback = nl_cb_alloc(NL_CB_DEBUG);
        if (!callback)
        {
            valid = false;
            break;
        }

        if (genl_connect(socket) < 0)
        {
            valid = false;
            break;
        }

        if ((nl80211_family_id = genl_ctrl_resolve(socket, "nl80211")) < 0)
        {
            valid = false;
            break;
        }

        valid = true;
    } while(0);

}

mynlret_t Socket::set_callback(nl_cb_t * cb)
{
    callback = cb;

    if (!valid)
    {
        return MYNL_ERROR;
    }

    nl_socket_set_cb(socket, callback);
    return MYNL_SUCCESS;
}

int Socket::get_family_id()
{
    return nl80211_family_id;
}

nl_sock_t * Socket::get_socket()
{
    return socket;
}
