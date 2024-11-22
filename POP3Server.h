#ifndef POP3SERVER_H_
#define POP3SERVER_H_
#include "tcpserver.h"
#include "POP3ServerSession.h"
#include "POP3ServerConfig.h"


class POP3Server: public TCPServer
{

public:
    POP3Server(unsigned short port=110);
    ~POP3Server();
protected:
    POP3ServerConfig* getServerConfig() {return (POP3ServerConfig*) conf;}
    void startNewSession(TcpSocket slave);
    void initCmd();
private:
    int readCmd(TcpSocket& slave, string& cmdLine);
    unsigned short parseCmd(const string& cmdLine, string cmd_argv[], int& cmd_argc);
protected:
};
#endif
