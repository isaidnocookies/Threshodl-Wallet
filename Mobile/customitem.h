#ifndef CUSTOMITEM_H
#define CUSTOMITEM_H

#include <QString>

class CustomItem
{
private:
    int amount;
    QString address;
    int generateRandomAmount() const;
    QString generateRandomString() const;

public:
    CustomItem();
    CustomItem(int amount, QString address);
    int getAmount();
    QString getAddress();
};

#endif // CUSTOMITEM_H
