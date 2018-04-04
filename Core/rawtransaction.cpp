#include "rawtransaction.h"

RawTransaction::RawTransaction(QMap< int, QString >* inputs, QMap< int, QString >* outputs)
{
    this->inputs = inputs;
    this->outputs = outputs;
}
