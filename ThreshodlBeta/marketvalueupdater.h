#ifndef MARKETVALUEUPDATER_H
#define MARKETVALUEUPDATER_H

#include <QObject>

class MarketValueUpdater : public QObject
{
    Q_OBJECT
public:
    explicit MarketValueUpdater(QObject *parent = nullptr);

signals:

public slots:
};

#endif // MARKETVALUEUPDATER_H
