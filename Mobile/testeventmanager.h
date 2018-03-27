#ifndef TESTEVENTMANAGER_H
#define TESTEVENTMANAGER_H

#include "customitem.h"
#include <QList>

class TestEventManager
{
private:
    QList<CustomItem> list;

public:
    TestEventManager();
    void addItem();
    void removeLast();
    int getTotal();
};

#endif // TESTEVENTMANAGER_H
