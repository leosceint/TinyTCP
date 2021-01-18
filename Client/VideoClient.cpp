#include "VideoClient.hpp"

#include <iostream>

using namespace std;

VideoClient::VideoClient(string hostname, int port):
    m_hostname(hostname),
    m_port(port),
    bRun(true),
    m_buffer_size(512),
    m_connection_thread(0),
    m_recv_thread(0)
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

int VideoClient::recv_img(string& img, const string command, const string recv_key)
{
    // посылаем запрос
    int send_result = send(connection, command.data(), command.size(), 0);
    if(send_result == SOCKET_ERROR)
        return -1;
    cout << "<WE SEND COMMAND -- " << command << " > " << endl;

    // принимаем ключ
    char* key = new char[recv_key.size()];
    int recv_len = recv(connection, key, recv_key.size(), 0);
    
    if(recv_len > 0) 
    {
        if(strncmp(key, recv_key.data(), recv_key.size()) == 0)
        {
            cout << "<WE GET KEY -- " << recv_key << " > " << endl;
            // принимаем размер
            int image_size;
            recv_len = recv(connection, reinterpret_cast<char*>(&image_size), sizeof(image_size), 0);
            if(recv_len > 0)
            {
                cout << "<WE GET SIZE OF DATA -- " << image_size << " > " << endl; 
                // принимаем данные
                char* image_buffer = new char[image_size];
                recv_len = recv(connection, image_buffer, image_size, 0);
                if(recv_len > 0)
                {
                    cout << "<WE GET DATA>" << endl;
                    img = string(image_buffer, image_size);
                    return recv_len;
                }
                else
                    return -1;
            }
            else
                return -1;
        }
    }
    return -1;
}

void VideoClient::connection_thread_worker()
{
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
        string img;

        while(connection_result == 0)
        {
            cout << endl << "Waiting for data" << endl;
            int recv_len;
            
            recv_len = recv_img(img);
            
            if(recv_len > 0)
            {
                m_mutex.lock();
                m_images.push(&img);
                m_mutex.unlock();
            }
            else
            {
                cout << endl << "Connection closed" << endl;
                connection_result = -1;
            }
        }

        closesocket(connection);
        WSACleanup();

    }
}

void VideoClient::recv_thread_worker()
{

}

string VideoClient::pop_image()
{
    string output;

    if(!m_images.empty())
    {
        m_mutex.lock();
        output = *(m_images.front());
        m_images.pop();
        m_mutex.unlock();
    }

    return output;
}

void VideoClient::start()
{
    cout << endl << "Start" << endl;

    m_connection_thread = new thread(&VideoClient::connection_thread_worker, this);
    m_connection_thread->detach();

}

void VideoClient::stop()
{
    cout << endl << "Stop" << endl;

    if (bRun) 
        bRun = false;
}
