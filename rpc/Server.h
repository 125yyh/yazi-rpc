#pragma once

#include <string>
using namespace std;

#include "Singleton.h"
using namespace yazi::utility;

#include "FunctionHandler.h"

namespace yazi {
namespace rpc {

class Server
{
public:
    Server();
    ~Server();

    void listen(const string & ip, int port);
    void start();
    void set_threads(int threads);
    void set_connects(int connects);
    void set_wait_time(int wait_time);

    template <typename F>
    void bind(const string & name, F func)
    {
        m_handler.bind(name, func);
    }

    void call(const string & name, DataStream & in, DataStream & out)
    {
        m_handler.call(name, in, out);
    }

private:
    string m_ip;
    int m_port;
    int m_threads;
    int m_connects;
    int m_wait_time;

    FunctionHandler m_handler;
};

}}
