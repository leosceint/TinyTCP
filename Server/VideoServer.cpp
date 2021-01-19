#include "VideoServer.hpp"

#include <iostream>

using namespace std;

VideoServer::VideoServer(int port):
    m_port(port),
    bRun(true),
    m_connection_thread(0),
    m_sending_thread(0)
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

// Для передачи одной картинки по протоколу (Описание протокола см. в Readme)
int VideoServer::send_image(const string& image, const string recv_command, const string key)
{
    char* command = new char[recv_command.size()];
    int send_result = 1;

    // принимаем команду от клиента (SENDIMG по умолчанию)
    int recv_len = recv(m_connection_socket, command, recv_command.size(), 0); 
    
    if(recv_len > 0)
    {
        if(strncmp(command, recv_command.data(), recv_command.size()) == 0)
        {
            cout << "<WE GET COMMAND -- " << recv_command << " > " << endl;
            
            // посылаем ключ клиенту (IMG по умолчанию)
            send_result = send(m_connection_socket, key.data(), key.size(), 0);
            if(send_result == SOCKET_ERROR)
                return -1;
            cout << "<WE SEND KEY -- " << key << " > " << endl;

            int image_size = image.size();
            // посылаем размер данных клиенту (в формате 4-байтовый int)
            send_result = send(m_connection_socket, reinterpret_cast<char*>(&image_size), sizeof(image_size), 0);
            if(send_result == SOCKET_ERROR)
                return -1;
            cout << "<WE SEND SIZE OF DATA -- " << image_size << " > " << endl; 

            // посылаем данные клиенту в соответствии с размером
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

// Worker потока для общего сетевого взаимодействия
void VideoServer::connection_thread_worker()
{   
    while(bRun)
    {
        init_WSA();

        int sinlen = sizeof(SOCKADDR_IN);

        if(bind(m_listen_socket, reinterpret_cast<sockaddr*>(&addr), sinlen) == SOCKET_ERROR)
        {
            m_mutex.lock();
            perror("Listen Socket bind failed: ");
            WSACleanup();
            m_mutex.unlock();
            continue;
        }

        if(listen(m_listen_socket, SOMAXCONN) != SOCKET_ERROR)
        {
            m_mutex.lock();
            cout << "Start listening at port: " << ntohs(addr.sin_port) << endl;
            m_mutex.unlock();
        }
        else
        {
            m_mutex.lock();
            perror("Start listening failed: ");
            WSACleanup();
            m_mutex.unlock();
            continue;
        }

        SOCKADDR_IN addr_accept;
        int sinlen_accept = sizeof(addr_accept);

        if((m_connection_socket = accept(m_listen_socket, reinterpret_cast<sockaddr*>(&addr_accept), &sinlen_accept)) < 0)
        {
            m_mutex.lock();
            perror("Error accept connection: ");
            WSACleanup();
            m_mutex.unlock();
            continue;
        }
        else
        {
            m_mutex.lock();
            cout << "Connected!" << endl;
            m_mutex.unlock();
        }

        sending_thread_worker();

        closesocket(m_listen_socket);
        WSACleanup();
    }
}

// Worker для общей отправки
void VideoServer::sending_thread_worker()
{
    // пересылка данных
    int send_result = 1;
    while(send_result != SOCKET_ERROR)
    {
        if(!m_images.empty())
        {
            string buffer = *(m_images.front());
            m_images.pop();
            send_result = send_image(buffer);
        }
    }
}

// Для запуска сервера
void VideoServer::start()
{
    cout << endl << "Start" << endl;

    m_connection_thread = new thread(&VideoServer::connection_thread_worker, this);
    m_connection_thread-> detach();
}

// Для останова сервера. Явно вызывать не требуется
void VideoServer::stop()
{
    cout << endl << "Stop" << endl;

    if(bRun)
        bRun = false;

}

// Для передачи данных во внутреннюю очередь сервера
void VideoServer::push_image(string* image)
{
    m_mutex.lock();
    m_images.push(image);
    m_mutex.unlock();
}
