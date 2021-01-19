#ifndef VIDEO_CLIENT_HPP
#define VIDEO_CLIENT_HPP

#include <thread>
#include <mutex>
#include <queue>
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

    thread* m_connection_thread;
    thread* m_recv_thread;
    mutex m_mutex;

    queue<string*> m_images;

    bool bRun;

    WSADATA WSAData;
    SOCKET connection;
    SOCKADDR_IN addr;

public:
    VideoClient(string hostname = "127.0.0.1", int port=9999);
    ~VideoClient();

    // Для запуска клиента
    void start();

    // Для остановки клиента. Явно вызывать не требуется
    void stop();

    // Для извлечения данных из внутренней очереди сервера
    string pop_image();

protected:
    // Worker потока для общего сетевого взаимодействия
    void connection_thread_worker();

private:
    void init_WSA();
    
    // Для получения одной картинки по протоколу (Описание протокола см. в Readme)
    int recv_img(string& img, const string command = "SENDIMG", const string recv_key = "IMG");
};

#endif