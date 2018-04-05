#include "useraccount.h"

UserAccount::UserAccount(QString iUsername)
{
    mUsername = iUsername;
}

QString UserAccount::getUsername()
{
    return mUsername;
}

QString UserAccount::getPublicKey()
{
    return mPublicKey;
}

QString UserAccount::getPrivateKey()
{
    return mPrivateKey;
}

void UserAccount::setUsername(QString iUsername)
{
    mUsername = iUsername;
}

void UserAccount::setKeys(QString iPub, QString iPriv)
{
    mPublicKey = iPub;
    mPrivateKey = iPriv;
}
