#include "Server.h"
using namespace yazi::rpc;

#include "System.h"
#include "Logger.h"
#include "IniFile.h"
#include "Singleton.h"
using namespace yazi::utility;

#include "TaskDispatcher.h"
using namespace yazi::thread;

#include "SocketHandler.h"
using namespace yazi::socket;

Server::Server()
{
    System * sys = Singleton<System>::instance();
    sys->init();

    // init logger
    Logger::instance()->open(sys->get_root_path() + "/log/server.log");

    // init inifile
    IniFile * ini = Singleton<IniFile>::instance();
    ini->load(sys->get_root_path() + "/config/server.ini");

    m_ip = (string)(*ini)["server"]["ip"];
    m_port = (*ini)["server"]["port"];
    m_threads = (*ini)["server"]["threads"];
    m_connects = (*ini)["server"]["max_conn"];
    m_wait_time = (*ini)["server"]["wait_time"];
}

Server::~Server()
{
}

void Server::listen(const string & ip, int port)
{
    m_ip = ip;
    m_port = port;
}

void Server::start()
{
    // init the thread pool and task queue
    TaskDispatcher * dispatcher = Singleton<TaskDispatcher>::instance();
    dispatcher->init(m_threads);

    // init the socket handler
    SocketHandler * socket_handler = Singleton<SocketHandler>::instance();
    socket_handler->listen(m_ip, m_port);
    socket_handler->handle(m_connects, m_wait_time);
}

void Server::set_threads(int threads)
{
    m_threads = threads;
}

void Server::set_connects(int connects)
{
    m_connects = connects;
}

void Server::set_wait_time(int wait_time)
{
    m_wait_time = wait_time;
}

