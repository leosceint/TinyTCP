#ifndef VIDEO_SERVER_HPP
#define VIDEO_SERVER_HPP

#include <vector>
#include <string>
#include <WinSock2.h>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

class VideoServer
{
private:
    int m_port;
    
    WSADATA WSAData;
    SOCKET m_listen_socket;
    SOCKET m_connection_socket;

    vector<string*> m_images;

    SOCKADDR_IN addr;

    bool bRun;

public:
    VideoServer(int port = 9999);
    ~VideoServer();

    void start();
    void stop();

    // void push_image(string* image);

private:
    void init_WSA();
    int send_image(const string& image, const string recv_command = "SENDIMG", const string key = "IMG");
};

#endif