#include<bits/stdc++.h>
#include "POP3ServerCLI.h"
using namespace std;
POP3ServerCLI::POP3ServerCLI():CmdLineInterface("POP3 Server>"){
    cout << "Simple POP3 Server" << endl;
    initCmd();
    server = new POP3Server();
    if(!server->configServer())
    {
        cerr << "Error: Cannot load server configuration" << endl;
        delete server;
        exit(-1);
    }
}
POP3ServerCLI::~POP3ServerCLI(){
    delete server;
}
void POP3ServerCLI::initCmd(){
    addCmd("start",CLI_CAST(&POP3ServerCLI::doStart));
    addCmd("stop",CLI_CAST(&POP3ServerCLI::doStop));
    addCmd("status",CLI_CAST(&POP3ServerCLI::doStatus));
    addCmd("resett",CLI_CAST(&POP3ServerCLI::doRestart));
    addCmd("help",CLI_CAST(&POP3ServerCLI::doHelp));
}
void POP3ServerCLI::doStart(string cmd_argv[],int cmd_argc){
    if(server->isRunning()){
        cout<<"POP3 Server is running!"<<endl;
    }else if(server->start()){
        cout<<"POP3 Server started"<<endl;
    }else{
        cout<<"POP3 Server fail to start"<<endl;
    }
}
void POP3ServerCLI::doStop(string cmd_argv[],int argc){
    server->stop();
    cout<<"POP3 server stopped"<<endl;
}
void POP3ServerCLI::doStatus(string cmd_argv[],int argc){
    if(server->isRunning()){
        cout<<"POP3 Server is running"<<endl;
    }else{
        cout<<"POP3 Server stopped"<<endl;
    }
}
void POP3ServerCLI::doRestart(string cmd_argv[],int argc){
    if(server->restart()){
        cout<<"POP3 Server restarted"<<endl;
    }else{
        cout<<"POP3 Server fail to start"<<endl;
    }
}
void POP3ServerCLI::doHelp(string cmd_argv[],int argc){
    cout << "Cac lenh cua chuong trinh:" << endl;
    cout << "- start               Bat POP3 server" << endl;
    cout << "- stop                Tat POP3 server" << endl;
    cout << "- status              Trang thai POP3 server" << endl;
    cout << "- help                Tro giup" << endl;
    cout << "- quit                Thoat" << endl;
}
