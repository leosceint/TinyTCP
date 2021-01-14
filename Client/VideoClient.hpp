#ifndef VIDEO_CLIENT_HPP
#define VIDEO_CLIENT_HPP

#include <vector>
#include <string>
#include <WinSock2.h>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

class VideoClient
{
private:
    string m_hostname;
    int m_port;
    int m_buffer_size;

    vector<string*> m_images;

    bool bRun;

    WSADATA WSAData;
    SOCKET connection;
    SOCKADDR_IN addr;

public:
    VideoClient(string hostname = "127.0.0.1", int port=9999);
    ~VideoClient();

    void start();
    void stop();
    // string pop_image();

private:
    void init_WSA();
    int recv_img(string* img, const string command = "SENDIMG", const string recv_key = "IMG");
};

#endif