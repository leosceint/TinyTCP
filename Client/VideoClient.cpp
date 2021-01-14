#include "VideoClient.hpp"

#include <iostream>

using namespace std;

VideoClient::VideoClient(string hostname, int port):
    m_hostname(hostname),
    m_port(port),
    bRun(true),
    m_buffer_size(512)
{

}

VideoClient::~VideoClient()
{
    stop();
}

void VideoClient::init_WSA()
{
    WSAStartup(MAKEWORD(2, 0), &WSAData);
    connection =  socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_addr.s_addr = inet_addr(m_hostname.data());
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
}

int VideoClient::recv_img(string* img, const string command, const string recv_key)
{
    // посылаем запрос
    int send_result = send(connection, command.data(), command.size(), 0);
    if(send_result == SOCKET_ERROR)
        return -1;
    // принимаем ключ
    char* key = new char[recv_key.size()];
    
    int recv_len = recv(connection, key, recv_key.size(), 0);
    if(recv_len > 0) 
    {
        if(strncmp(key, recv_key.data(), recv_key.size()) == 0)
        {
            // принимаем размер
            char* c_image_size = new char[];
            recv_len = recv(connection, c_image_size, ? , 0);
            if(recv_len > 0)
            {
                int image_size = atoi(c_image_size);
                // принимаем данные
                char* image_buffer = new char[image_size];
                recv_len = recv(connection, image_buffer, image_size, 0);
                if(recv_len > 0)
                {
                    img = new string(image_buffer, image_size);
                    return recv_len;
                }
                else
                    return -1;
            }
            else
                return -1;
            

        }
    }
    else 
        return -1;
}

void VideoClient::start()
{
    cout << endl << "Start" << endl;

    while(bRun)
    {
        init_WSA();
        int connection_result = -1;
        int connection_count = 5;
        
        // подключаемся
        while(connection_result != 0)
        {
            connection_result = connect(connection, (SOCKADDR*)&addr, sizeof(addr));
            
            if(connection_result == 0)
                cout << endl << "Connected to server!" << endl;
            else
            {
                cout << "Try to connection ... " << endl;
                --connection_count;
                if(connection_count < 0)
                {
                    cout << "Connection pause ..." << endl;
                    Sleep(1500);
                    connection_count = 5;
                }
            }
        }

        // получаем данные из сокета
        char* buffer = new char[m_buffer_size];
        while(connection_result == 0)
        {
            cout << endl << "Waiting for data" << endl;
            int recv_len;
            recv_len = recv(connection, buffer, m_buffer_size, 0);
            
            if(recv_len > 0)
            {
                cout << "<WE GET DATA>" << endl;
                //int data_len = strlen(buffer);
                string* img = new string(buffer, recv_len);
                m_images.push_back(img);
                cout << *img << endl;
                cout << "============" << endl;
            }
            else
            {
                cout << endl << "Connection closed" << endl;
                connection_result = -1;
            }
        }
        delete[] buffer;

        closesocket(connection);
        WSACleanup();

    }
}

void VideoClient::stop()
{
    cout << endl << "Stop" << endl;

    if (bRun) 
        bRun = false;
}
