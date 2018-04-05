#ifndef RAWTRANSACTION_H
#define RAWTRANSACTION_H

#include <QMap>
#include <QString>


class RawTransaction
{
public:
    RawTransaction(QMap< int, QString >*   inputs,QMap< int, QString >*   outputs);
    QMap< int, QString >*   inputs;
    QMap< int, QString >*   outputs;
};

#endif // RAWTRANSACTION_H
