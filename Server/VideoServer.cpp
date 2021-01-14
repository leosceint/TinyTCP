#include "VideoServer.hpp"

#include <iostream>

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

int VideoServer::send_image(const string& image, const string recv_command, const string key)
{
    char* command = new char[recv_command.size()];
    int send_result = 1;

    // принимаем команду
    int recv_len = recv(m_connection_socket, command, recv_command.size(), 0); 
    
    if(recv_len > 0)
    {
        if(strncmp(command, recv_command.data(), recv_command.size()) == 0)
        {
            cout << "<WE GET COMMAND -- " << recv_command << " > " << endl;
            
            // посылаем ключ
            send_result = send(m_connection_socket, key.data(), key.size(), 0);
            if(send_result == SOCKET_ERROR)
                return -1;
            cout << "<WE SEND KEY -- " << key << " > " << endl;

            int image_size = image.size();
            // посылаем размер данных
            send_result = send(m_connection_socket, reinterpret_cast<char*>(&image_size), sizeof(image_size), 0);
            if(send_result == SOCKET_ERROR)
                return -1;
            cout << "<WE SEND SIZE OF DATA -- " << image_size << " > " << endl; 

            // посылаем данные
            send_result = send(m_connection_socket, image.data(), image.size(), 0);
            
            if(send_result == SOCKET_ERROR)
                return -1;
            else
            {
                cout << "<WE SEND DATA>" << endl;
                return send_result;
            }
        }
    }
    return -1;
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
            string buffer = "hello world !!!";
            send_result = send_image(buffer);
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
