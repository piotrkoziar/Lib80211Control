
#include "Message.h"

using namespace mynl;

class MynlBaseInstance
{
public:
    void Main()
    {
        Socket sock;
        Wiphy * wiphy = new Wiphy("phy0");
        Message msg(wiphy, Attribute::Command::Get, NULL);

    }
};

int main(int argc, char ** argv)
{
    MynlBaseInstance mynl_base;
    mynl_base.Main();

    return 0;
}