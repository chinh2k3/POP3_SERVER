#include <iostream>
#include<bits/stdc++.h>
#include "POP3ServerSession.h"
#include "POP3ServerConfig.h"

#include <unistd.h>

//to_string() : chuyển các kiểu dữ liệu khác thành string
POP3ServerSession::POP3ServerSession(const TcpSocket& slave,ServerConfig* conf):Session(slave,conf)
{
    this->info = new POP3ServerSessionInfo();
    this->quitSession = false;
    this->info->status = STATUS_INIT;
}

void POP3ServerSession::reset()
{
    this->info->status = STATUS_INIT;
    this->info->fromAddress.clear();
    this->info->toAddress.clear();
}
string POP3ServerSession::processString(string str){//sử lý chuỗi
    string result;
    for(int i=0;i<str.size();i++){
        if(str[i]=='*'){//nếu gặp ký tự *
            result +=' ';//thêm dấu cách đằng trước
        }
        result +=str[i];
        if(isdigit(str[i])){//nếu gặp số
            result +=' ';//thêm dấu cách đằng sau
        }
    }
    return result;
}
POP3ServerSession::~POP3ServerSession()
{
    delete info;
}

void POP3ServerSession::doUser(string cmd_argv[],int cmd_argc){
    if(cmd_argc==2){
        ifstream user;
        user.open("login.txt",ios::in);
        string line;
        string username,password;
        bool found=false;//kiểm tra xem có tìm được user hay không
        //getline đọc dữ liệu từng dòng
        while(getline(user,line)){
            stringstream iss(line);
            iss>>username>>password;
            if(cmd_argv[1]==username){
                response = "+OK "+username+" is a valid mailbox\r\n";
                slave.send(response);
                logUserName = username;//gán giá trị user tìm được vào biến logUserName
                logPassWord = password;//gán password tìm được vào biến logPassWord
                found=true;//nếu tìm thấy sẽ gán giá trị biến đánh dấu là true
                break;//thoát khỏi vòng lặp tìm kiếm
            }
        }
        user.close();
        if(!found){
            response = "-ERR nerver heard of mailbox "+cmd_argv[1]+" \r\n";
            slave.send(response);
        }
    }
    else{
        response = "-ERR nerver heard of mailbox\r\n";
        slave.send(response);
    }

}
void POP3ServerSession::doPassword(string cmd_argv[],int cmd_argc){
    if(cmd_argc==2){
        if(cmd_argv[1]==logPassWord){//kiểm tra xem password nhập vào có trùng với logPassWord hay không
            response = "+OK Maildrop locked and ready\r\n";
            slave.send(response);
        }else{
             response =  "-ERR invalid password\r\n";
             slave.send(response);
        }
    }
    else{
        response = "-ERR invalid password\r\n";
        slave.send(response);
    }

}
void POP3ServerSession::doStat(string cmd_argv[], int cmd_argc)
{
    if(cmd_argc==1)
    {
        ifstream mail;
        mail.open("email.txt",ios::in);

        string line;
        string email,username,id;

        int total=0;//tổng số email mà user có
        size_t octet=0;//tổng số octet của tất cả email đó
        while(getline(mail,line)){
            stringstream iss(line);
            iss>>id>>username>>email;
            if(username == logUserName){
                octet += sizeof(email);
                total++;
            }
        }
        response = "+OK "+to_string(total)+" "+to_string(octet) +"\r\n";
        slave.send(response);
        mail.close();

    }
    else
    {
        response = "-ERR\r\n";
        slave.send(response);
    }
}

void POP3ServerSession::doList(string cmd_argv[], int cmd_argc)
{
    if(cmd_argc==1)
    {
        ifstream mail;
        mail.open("email.txt",ios::in);
        string line;
        string username,email,id;
        int count=0;//đếm số lượng email
        size_t total=0;//tính tổng số lượng octet email mà user có
        while(getline(mail,line)){
            stringstream iss(line);//cắt các biến trên dòng line
            iss>>id>>username>>email;//truyền dữ liệu vào các biến
            if(username == logUserName){
                total += sizeof(email);
                count++;
                response = to_string(count)+" "+to_string(sizeof(email))+"\r\n";//trả về số dòng và số octet của 1 mail
                slave.send(response);
            }
        }
        string msg = "+OK "+to_string(count)+" message "+to_string(total)+" octet \r\n";//trả về tổng số dòng và số octet của tất cả các mail
        slave.send(msg);
        mail.close();
    }
    else if(cmd_argc==2){
        ifstream mail;
        mail.open("email.txt",ios::in);

        string line;
        string username,email,id;

        while(getline(mail,line)){
            stringstream iss(line);//cắt các biến trên dòng line
            iss>>id>>username>>email;//truyền dữ liệu vào các biến
            if(username == logUserName&&cmd_argv[1]==id){//Kiểm tra xem tên và id mail đúng chưa
                response = id+" "+to_string(sizeof(email))+"\r\n";//trả về số thứ tự và dung lượng của mail
                slave.send(response);
            }
        }
        mail.close();
    }
    else
    {
        response = "-ERR\r\n";
        slave.send(response);
    }

}
void POP3ServerSession::doRetr(string cmd_argv[], int cmd_argc)
{
    if(cmd_argc==2){
        ifstream mail;
        mail.open("email.txt",ios::in);

        string line;
        string username,email,id;

        while(getline(mail,line)){
            stringstream iss(line);//cắt các biến trên dòng line
            iss>>id>>username>>email;//truyền dữ liệu vào các biến
            if(username == logUserName&& cmd_argv[1]==id){//Kiểm tra xem tên và id mail đúng chưa
                string msg="+OK "+to_string(sizeof(email))+" octet\r\n";//hàm sizeof() trả về giá trị của 1 chuỗi dưới dạng byte
                slave.send(msg);
                response="<"+email+">\r\n";//lấy ra nội dung của email
                slave.send(response);
            }
        }
        mail.close();
    }
    else{
        response = "-ERR\r\n";
        slave.send(response);
    }
}
void POP3ServerSession::doDelete(string cmd_argv[],int cmd_argc)
{
    if(cmd_argc==2){
        ifstream mail;
        ofstream deleMail;

        mail.open("email.txt",ios::in);
        deleMail.open("deleteMail.txt",ios::out);

        string line;
        string username,email,id;
        string delemail,deleid,deleuser;

        while(getline(mail,line)){
            stringstream iss(line);//cắt các biến trên dòng line
            iss>>id>>username>>email;
            if(username==logUserName&&cmd_argv[1]==id){//Kiểm tra xem dòng cần đánh dấu
                delemail=email;//Gán dữ liêu của file cần đánh dấu vào biến tạm
                deleuser = username;//Gán dữ liêu của file cần đánh dấu vào biến tạm
                deleid = id;//Gán dữ liêu của file cần đánh dấu vào biến tạm
            }
        }
        deleMail<<deleid<<" "<<deleuser<<" "<<delemail<<"\n";//đẩy file cần đánh dấu vào file xóa

        response="+OK message 1 deleted\r\n";
        slave.send(response);
        deleMail.close();
        mail.close();
    }else{
        response = "-ERR no such message\r\n";
        slave.send(response);
    }
}
void POP3ServerSession::doRset(string cmd_argv[], int cmd_argc)
{
    if(cmd_argc==1){
        ofstream deleMail;
        deleMail.open("deleteMail.txt",ios::trunc);//ios::trunc có nghĩa là xóa toàn bộ dữ liệu trong file txt
        response="+OK mailbox reset\r\n";
        slave.send(response);
        deleMail.close();
    }else{
        response="-ERR could not reset mailbox";
        slave.send(response);
    }
}

void POP3ServerSession::doNoop(string cmd_argv[], int cmd_argc)
{

}

void POP3ServerSession::doQuit(string cmd_argv[], int cmd_argc)
{
    if(cmd_argc==1){
        ifstream email;
        ifstream deleMail;
        ofstream temp;
        email.open("email.txt",ios::in);
        deleMail.open("deleteMail.txt",ios::in);
        temp.open("temp.txt",ios::out);

        string line1,line2;
        string deleLine;

        unordered_set<string> Email;//tạo 1 unordered_set O(1)
        while(getline(email,line1)){
            line1.erase(remove(line1.begin(),line1.end(),' '),line1.end());//xóa khoảng trắng của các dòng
            Email.insert(line1);
        }


        while(getline(deleMail,line2)){
            line2.erase(remove(line2.begin(),line2.end(),' '),line2.end());//xóa khoảng trắng của các dòng
            if(Email.find(line2) != Email.end()){//kiểm tra dòng xóa có tồn tại trong set Email hay không (Email.end() đại diện cho không tìm thấy)
                deleLine = line2;
            }
        }
        for(auto item : Email){
            if(item != deleLine){//Kiểm tra xem email trong danh sách khác với email cần xóa
               temp<<this->processString(item)<<endl;//Thêm email trừ email cần xóa vào file tạm
            }
        }

        email.close();
        deleMail.close();
        temp.close();
        remove("email.txt");//xóa file email
        if(rename("temp.txt","email.txt")==0){//đổi tên file
            response = "+OK POP3 Server signing off\r\n";
            slave.send(response);
        }else{
            response = "Fail\r\n";
            slave.send(response);
        }

    }else{
        response = "-ERR";
        slave.send(response);
    }
}


void POP3ServerSession::doUnknown(string cmd_argv[], int cmd_argc)
{
    response = "500 Unknown command\r\n" ;
    slave.send(response);
}
