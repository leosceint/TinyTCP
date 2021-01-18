#include <iostream>

#include "VideoClient.hpp"

using namespace std;

int main()
{
    VideoClient client;
    client.start();
    string out;
    while(true)
    {
        out = client.pop_image();
        if(!out.empty())
        {
            cout << "CURRENT DATA --> " << out << endl;
            out.clear();
        }
    }
    return 0;
}