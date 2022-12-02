#include "WorkTask.h"
using namespace yazi::task;

#include "Logger.h"
#include "Singleton.h"
using namespace yazi::utility;

#include "SocketHandler.h"
using namespace yazi::socket;

#include "Server.h"
using namespace yazi::rpc;

#include "DataStream.h"
using namespace yazi::serialize;

#include <sstream>
using std::ostringstream;


WorkTask::WorkTask(Socket * socket) : Task(socket)
{
}

WorkTask::~WorkTask()
{
}

void WorkTask::run()
{
    debug("work task run");
    SocketHandler * handler = Singleton<SocketHandler>::instance();

    Socket * socket = static_cast<Socket *>(m_data);

    MsgHead msg_head;
    memset(&msg_head, 0, sizeof(msg_head));
    int len = socket->recv((char *)(&msg_head), sizeof(msg_head));
    if (len == 0)
    {
        warn("socket closed by peer");
        handler->remove(socket);
        return;
    }
    if (len == -1 && errno == EAGAIN)
    {
        warn("socket recv len: %d, error msg: EAGAIN errno: %d", len, errno);
        handler->attach(socket);
        return;
    }
    if (len == -1 && errno == EWOULDBLOCK)
    {
        warn("socket recv len: %d, error msg: EWOULDBLOCK errno: %d", len, errno);
        handler->attach(socket);
        return;
    }
    if (len == -1 && errno == EINTR)
    {
        warn("socket recv len: %d, error msg: EINTR errno: %d", len, errno);
        handler->attach(socket);
        return;
    }
    if (len != sizeof(msg_head))
    {
        error("recv msg head error length: %d, errno: %d", len, errno);
        handler->remove(socket);
        return;
    }
    info("recv msg head len: %d, flag: %s, cmd: %d, body len: %d", len, msg_head.flag, msg_head.cmd, msg_head.len);
    if (strncmp(msg_head.flag, "work", 4) != 0)
    {
        error("msg head flag error");
        handler->remove(socket);
        return;
    }

    if (msg_head.len >= uint32_t(recv_buff_size))
    {
        error("recv msg body len: %d, large than recv_buff_size: %d", msg_head.len, recv_buff_size);
        handler->remove(socket);
        return;
    }

    char buf[recv_buff_size];
    memset(buf, 0, recv_buff_size);
    len = socket->recv(buf, msg_head.len);
    if (len == -1 && errno == EAGAIN)
    {
        warn("socket recv len: %d, error msg: EAGAIN errno: %d", len, errno);
        handler->remove(socket);
        return;
    }
    if (len == -1 && errno == EWOULDBLOCK)
    {
        warn("socket recv len: %d, error msg: EWOULDBLOCK errno: %d", len, errno);
        handler->remove(socket);
        return;
    }
    if (len == -1 && errno == EINTR)
    {
        warn("socket recv len: %d, error msg: EINTR errno: %d", len, errno);
        handler->remove(socket);
        return;
    }
    if (len != (int)(msg_head.len))
    {
        error("recv msg body error length: %d, body: %s, errno: %d", len, buf, errno);
        handler->remove(socket);
        return;
    }

    info("recv msg body len: %d, msg data: %s", len, buf);

    DataStream in;
    in.write(buf, len);

    string func;
    in >> func;

    DataStream out;
    Server * server = Singleton<Server>::instance();
    server->call(func, in, out);
    socket->send(out.data(), out.size());
    handler->attach(socket);
}

void WorkTask::destroy()
{
    debug("work task destory");
    delete this;
}
