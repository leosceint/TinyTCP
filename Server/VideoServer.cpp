#include "VideoServer.hpp"

#include <iostream>
#include <string>

using namespace std;

VideoServer::VideoServer(int port):
    m_port(port),
    bRun(true)
{

}

VideoServer::~VideoServer()
{
    stop();
}

void VideoServer::init_WSA()
{
    WSAStartup(MAKEWORD(2, 0), &WSAData); 
    m_listen_socket = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_port = htons(m_port);
}

void VideoServer::start()
{
    cout << endl << "Start" << endl;
    
    while(bRun)
    {
        init_WSA();

        int sinlen = sizeof(SOCKADDR_IN);

        if(bind(m_listen_socket, reinterpret_cast<sockaddr*>(&addr), sinlen) == SOCKET_ERROR)
        {
            perror("Listen Socket bind failed: ");
            WSACleanup();
            continue;
        }

        if(listen(m_listen_socket, SOMAXCONN) != SOCKET_ERROR)
        {
            cout << "Start listening at port: " << ntohs(addr.sin_port) << endl;
        }
        else
        {
            perror("Start listening failed: ");
            WSACleanup();
            continue;
        }

        SOCKADDR_IN addr_accept;
        int sinlen_accept = sizeof(addr_accept);

        if((m_connection_socket = accept(m_listen_socket, reinterpret_cast<sockaddr*>(&addr_accept), &sinlen_accept)) < 0)
        {
            perror("Error accept connection: ");
            WSACleanup();
            continue;
        }
        else
        {
            cout << "Connected!" << endl;
        }

        int send_result = 1;
        while(send_result != SOCKET_ERROR)
        {
            // string buffer = "hello world !!!";
            int number = 134;
            send_result = send(m_connection_socket, reinterpret_cast<char*>(&number), sizeof(number), 0);
            cout << "Data send" << endl;
        }

        closesocket(m_listen_socket);
        WSACleanup();

    }
}

void VideoServer::stop()
{
    cout << endl << "Stop" << endl;

    if(bRun)
        bRun = false;

}
