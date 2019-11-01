
#include "Message.h"

using namespace mynl;

class MynlBaseInstance
{
public:
    void Main()
    {
        Socket * sock = new Socket();
        Wiphy * wiphy = new Wiphy("phy0");
        Message * msg = new Message();
        msg->prepare_message(wiphy, Attribute::Command::Get, NULL);
        msg->send(sock);
    }
};

int main(int argc, char ** argv)
{
    MynlBaseInstance mynl_base;
    mynl_base.Main();

    return 0;
}