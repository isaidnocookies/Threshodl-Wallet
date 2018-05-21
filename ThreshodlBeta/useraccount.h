#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <QObject>

class UserAccount : public QObject
{
    Q_OBJECT

public:
    explicit UserAccount(QObject *parent = nullptr);
};

#endif // USERACCOUNT_H
