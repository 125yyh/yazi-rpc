#include "Client.h"
using namespace yazi::rpc;

#include "System.h"
#include "Logger.h"
#include "Singleton.h"
using namespace yazi::utility;


Client::Client()
{
    System * sys = Singleton<System>::instance();
    sys->init();

    // init logger
    Logger::instance()->open(sys->get_root_path() + "/log/client.log");
}

Client::~Client()
{
    m_socket.close();
}

bool Client::connect(const string & ip, int port)
{
    m_ip = ip;
    m_port = port;
    return m_socket.connect(ip, port);
}