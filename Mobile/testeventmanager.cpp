#include "testeventmanager.h"

TestEventManager::TestEventManager() {
    this->list = QList<CustomItem>();
}

void TestEventManager::addItem() {
    CustomItem item = CustomItem::CustomItem();
    this->list.append(item);
}

void TestEventManager::removeLast() {
    this->list.removeLast();
}

int TestEventManager::getTotal() {
    int sum = 0;
    foreach (CustomItem item, this->list) {
        sum += item.getAmount();
    }
    return sum;
}
