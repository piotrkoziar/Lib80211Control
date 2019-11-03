
#include "Message.h"
#include <iostream>

using namespace wiphynlcontrol;
using namespace std;

class MynlBaseInstance
{
public:
    void Main()
    {
        try
        {
            Socket * sock = new Socket();
            Wiphy * wiphy = new Wiphy("phy0");
            cout << wiphy->name_ << "\n";
            Message * msg = new Message();
            msg->prepare_message(wiphy, Entity::Commands::Get, NULL);
            msg->send(sock);
            cout << wiphy->id_ << "\n";
            cout << wiphy->frequency_ << "\n";
        }
        catch (exception &e)
        {
            cout << "Exception: " << e.what() << "\n";
        }
    }
};

int main(int argc, char ** argv)
{
    MynlBaseInstance mynl_base;
    mynl_base.Main();

    return 0;
}