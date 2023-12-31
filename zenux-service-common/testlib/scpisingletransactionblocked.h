#ifndef SCPISINGLETRANSACTIONBLOCKED_H
#define SCPISINGLETRANSACTIONBLOCKED_H

#include <QString>

class ScpiSingleTransactionBlocked
{
public:
    static QString query(QString scpiQuery);
    static QString cmd(QString scpiCmd, QString param);
};

#endif // SCPISINGLETRANSACTIONBLOCKED_H
