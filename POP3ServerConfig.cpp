#include<bits/stdc++.h>
#include "POP3ServerConfig.h"
POP3ServerAccount::POP3ServerAccount(){
}
POP3ServerAccount::~POP3ServerAccount(){
}
POP3ServerAccount::POP3ServerAccount(string username,string password):Account(username,password){
}
string POP3ServerAccount::getUserName(){
    return this->username;
}

POP3ServerConfig::POP3ServerConfig(){

}
POP3ServerConfig::~POP3ServerConfig(){

}
bool POP3ServerConfig::isValidEmailAccount(const string& email)
{
    return isValidUser(email);
}
void POP3ServerConfig::setMailBox(const string& mailBox)
{
    this->mailBox = mailBox;
}

const string& POP3ServerConfig::getMailBox() const
{
    return mailBox;
}
bool POP3ServerConfig::loadAccountsFromFile(const string& fileName)
{
    return false;

}
