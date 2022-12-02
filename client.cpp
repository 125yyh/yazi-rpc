#include <iostream>
using namespace std;

#include "Client.h"
using namespace yazi::rpc;

class Request : public Serializable
{
public:
    Request() {}
    Request(const string & name) : m_name(name) {}
    ~Request() {}

    const string & name() const
    {
        return m_name;
    }

    SERIALIZE(m_name)

private:
    string m_name;
};

class Response : public Serializable
{
public:
    Response() {}
    Response(const string & name) : m_name(name) {}
    ~Response() {}

    const string & name() const
    {
        return m_name;
    }

    SERIALIZE(m_name)

private:
    string m_name;
};

int main()
{
    Client client;
    client.connect("127.0.0.1", 8080);

    auto reply = client.call<string>("hello", "kitty");
    std::cout << reply << std::endl;

    auto total = client.call<int>("sum", 1, 2);
    std::cout << total << std::endl;

    Request req("kitty");
    auto resp = client.call<Response>("upper", req);
    std::cout << resp.name() << std::endl;
    
    return 0;
}