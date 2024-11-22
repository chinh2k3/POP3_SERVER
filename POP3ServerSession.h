#ifndef POP3SERVERSESSION_H_INCLUDED
#define POP3SERVERSESSION_H_INCLUDED
#include "session.h"
#include "tcpserversocket.h"
#include "POP3ServerConfig.h"
#include <cstdint>
#define STATUS_INIT     0
#define STATUS_FROM     1
#define STATUS_TO       2


struct POP3ServerSessionInfo
{
    uint8_t  status;
    string fromAddress;
    string toAddress;
};

class POP3ServerSession : public Session
{
    POP3ServerSessionInfo* info;
    string response;
    string logUserName;
    string logPassWord;
    void reset();
    string processString(string s);
    POP3ServerConfig* getPOP3ServerConfig(){return (POP3ServerConfig*)conf;}
public:
    POP3ServerSession(const TcpSocket& slave,ServerConfig* conf);
    ~POP3ServerSession();
    const string& getResponse() const {return response;}
    void doUser(string cmd_argv[], int cmd_argc);
    void doPassword(string cmd_argv[], int cmd_argc);
    void doStat(string cmd_argv[], int cmd_argc);
    void doList(string cmd_argv[], int cmd_argc);
    void doRetr(string cmd_argv[], int cmd_argc);
    void doDelete(string cmd_argv[], int cmd_argc);
    void doRset(string cmd_argv[], int cmd_argc);
    void doNoop(string cmd_argv[], int cmd_argc);
    void doQuit(string cmd_argv[], int cmd_argc);
    void doUnknown(string cmd_argv[], int cmd_argc);

};
#endif
