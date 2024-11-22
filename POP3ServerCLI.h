#include "cli.h"
#include "POP3Server.h"
#include <bits/stdc++.h>
class POP3ServerCLI : public CmdLineInterface{
private:
    POP3Server* server;
public:
    POP3ServerCLI();
    ~POP3ServerCLI();
protected:
    virtual void initCmd();
public:
    void doStart(string cmd_argv[], int cmd_argc);
    void doStop(string cmd_argv[], int cmd_argc);
    void doStatus(string cmd_argv[], int cmd_argc);
    void doRestart(string cmd_argv[], int cmd_argc);
    void doHelp(string cmd_argv[], int cmd_argc);
};
