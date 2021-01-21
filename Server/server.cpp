#include <iostream>
#include <fstream>
#include <string>

#include "VideoServer.hpp"

using namespace std;

int main()
{
    VideoServer server;
    server.start();

    string str;
    
    ifstream f_read("pack\\ToSend.txt");
    
    unsigned data_index = 1;
    while(getline(f_read, str))
    {
        server.push_image(&str);
        cout << "We push data #" << data_index++ << endl;
        Sleep(2000);
    }

    f_read.close();
    return 0;
}