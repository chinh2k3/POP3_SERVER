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
        char cmdBuffer[256];//tạo 1 mảng bộ nhớ đệm để lưu dữ liệu nhận
        int byteRead = client.recvLine(cmdBuffer,256);//nhận dữ liệu gửi đến từ bên client

        // Nếu đọc được ít nhất 2 byte (giả sử là chuỗi có ký tự kết thúc dòng CRLF "\r\n")
        if(byteRead >=2){
            cmdBuffer[byteRead-2]=0;//loại bỏ /r/n ở cuối dòng
            cmdLine = cmdBuffer;//chuyển dữ liệu từ buffer sang string
        }
        return byteRead;//trả về tổng số byte được nhận

    }catch(SocketException& ex){
        cerr<<ex.what()<<endl;
        return -1;
    }

}
unsigned short POP3Server::parseCmd(const string& cmdLine, string cmd_argv[], int& cmd_argc){
    cmd_argc=0;//số lượng tham số lệnh
    cmd_argv[cmd_argc].clear();//làm trống mảng chứa tham số

    istringstream iss(cmdLine);//cắt chuỗi lệnh và tham số
    while(cmd_argc<2 && iss.good()){//lặp tối đa 2 lần
        iss>>cmd_argv[cmd_argc];//truyền vào lệnh và tham số
        cmd_argc++;//tăng biến đếm trong mảng
    }

    //Kiểm tra lệnh nhập vào có trùng với lệnh trong NameList không
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
    //Tạo 1 session mới
    POP3ServerSession* session = new POP3ServerSession(client,conf);
    string cmdLine;
    string cmdArgv[SERVER_CMD_ARG_NUM];
    int cmdArgc;
    unsigned short cmdId;
    int cmdLen;
    try
    {
        client.send(POP3Server_GREETING);
        while(!session->isQuit())//Session không đóng
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
