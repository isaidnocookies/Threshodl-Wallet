#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <QString>

class UserAccount
{
public:
    UserAccount(QString iUsername);

    QString getUsername();
    QString getPublicKey();
    QString getPrivateKey();

    void setUsername(QString iUsername);
    void setKeys(QString iPub, QString iPriv);

private:
    QString mUsername;
    QString mPublicKey;
    QString mPrivateKey;
};

#endif // USERACCOUNT_H
