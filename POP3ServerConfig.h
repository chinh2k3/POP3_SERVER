#include "serverconfig.h"
#ifndef SMTPACCOUNT_H_
#define SMTPACCOUNT_H_
class POP3ServerConfig : public ServerConfig{
private:
    string mailBox;
public:
    POP3ServerConfig();
    ~POP3ServerConfig();
    void setMailBox(const string& mailBox);
    const string& getMailBox() const;
    bool loadAccountsFromFile(const string& fileName);
    bool isValidEmailAccount(const string& email);
};
#endif
