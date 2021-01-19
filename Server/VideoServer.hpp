#ifndef VIDEO_SERVER_HPP
#define VIDEO_SERVER_HPP

#include <queue>
#include <string>
#include <thread>
#include <mutex>
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

    mutex m_mutex;
    thread* m_connection_thread;
    thread* m_sending_thread;

    queue<string*> m_images;

    SOCKADDR_IN addr;

    bool bRun;

public:
    VideoServer(int port = 9999);
    ~VideoServer();

    // Для запуска сервера
    void start();

    // Для останова сервера. Явно вызывать не требуется
    void stop();

    // Для передачи данных во внутреннюю очередь сервера
    void push_image(string* image);

protected:
    // Worker потока для общего сетевого взаимодействия
    void connection_thread_worker();
    
    // Worker для общей отправки
    void sending_thread_worker();

private:
    void init_WSA();

    // Для передачи одной картинки по протоколу (Описание протокола см. в Readme)
    int send_image(const string& image, const string recv_command = "SENDIMG", const string key = "IMG");
};

#endif