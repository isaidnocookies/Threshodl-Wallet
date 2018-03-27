#include "customitem.h"

CustomItem::CustomItem() {
    this->amount = this->generateRandomAmount();
    this->address = this->generateRandomString();
}

CustomItem::CustomItem(int amount, QString address) {
    this->amount = amount;
    this->address = address;
}

QString CustomItem::getAddress() {
    return this->address;
}

int CustomItem::getAmount() {
    return this->amount;
}

QString CustomItem::generateRandomString() const
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
   const int randomStringLength = 12; // assuming you want random strings of 12 characters

   QString randomString;
   for(int i=0; i<randomStringLength; ++i)
   {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}

int CustomItem::generateRandomAmount() const
{
   return qrand() % 100;
}

