#include <bits/stdc++.h>
#include "POP3Server.h"
using namespace std;
const char POP3Server_GREETING[] = "+OK POP3 server ready\r\n";

POP3Server :: POP3Server(unsigned short port):TCPServer(port){
    initCmd();
    conf = new POP3ServerConfig();
}
POP3Server :: ~POP3Server(){
    if(isRunning())
    {
        stop();
        cout<< "POP3 Server stopped!\n";
    }
}
void POP3Server::initCmd(){
    addCmd("USER", FUNC_CAST(&POP3ServerSession::doUser));
    addCmd("PASSWORD", FUNC_CAST(&POP3ServerSession::doPassword));
    addCmd("STAT", FUNC_CAST(&POP3ServerSession::doStat));
    addCmd("LIST", FUNC_CAST(&POP3ServerSession::doList));
    addCmd("RETR", FUNC_CAST(&POP3ServerSession::doRetr));
    addCmd("DELETE", FUNC_CAST(&POP3ServerSession::doDelete));
    addCmd("RSET", FUNC_CAST(&POP3ServerSession::doRset));
    addCmd("NOOP", FUNC_CAST(&POP3ServerSession::doNoop));
    addCmd("QUIT", FUNC_CAST(&POP3ServerSession::doQuit));
}
//Đọc dữ liệu từ bên client
int POP3Server::readCmd(TcpSocket& client,string& cmdLine){
    try{
        char cmdBuffer[256];
        int byteRead = client.recvLine(cmdBuffer,256);
        if(byteRead >=2){
            cmdBuffer[byteRead-2]=0;
            cmdLine = cmdBuffer;
        }
        return byteRead;

    }catch(SocketException& ex){
        cerr<<ex.what()<<endl;
        return -1;
    }

}
unsigned short POP3Server::parseCmd(const string& cmdLine, string cmd_argv[], int& cmd_argc){
    cmd_argc=0;
    cmd_argv[cmd_argc].clear();
    istringstream iss(cmdLine);//cắt chuỗi
    while(cmd_argc<2 && iss.good()){
        iss>>cmd_argv[cmd_argc];
        cmd_argc++;
    }
    // check command name and return command ID
     if(cmd_argc>0 && !cmd_argv[0].empty())
    {
        for(int i = 0; i<this->numCmd; i++)
        {
            if(cmd_argv[0] == cmdNameList[i])
                return i ;
        }
    }
    return SERVER_CMD_UNKNOWN;
}
void POP3Server ::startNewSession(TcpSocket client){
    POP3ServerSession* session = new POP3ServerSession(client,conf);
    string cmdLine;
    string cmdArgv[SERVER_CMD_ARG_NUM];
    int cmdArgc;  // number of command arguments
    unsigned short cmdId; // ID of command
    int cmdLen; // length of a SMTP command
    try
    {
        client.send(POP3Server_GREETING);
        while(!session->isQuit())
        {
            // Nhan lenh
            cmdLen = readCmd(client,cmdLine);
            // Kiem tra lenh
            if(cmdLen <= 0)
                break;
            // Phan tich lenh
            cmdId = parseCmd(cmdLine, cmdArgv, cmdArgc );
            // Thuc hien lenh trong session
            doCmd(session,cmdId,cmdArgv,cmdArgc);
        }
        // session finish
        delete session;
    }
    catch(SocketException&e)
    {
        cerr << e.what() << endl;
        delete session;
    }
}