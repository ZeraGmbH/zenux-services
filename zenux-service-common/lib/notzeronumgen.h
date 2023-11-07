#ifndef NOTZERONUMGEN_H
#define NOTZERONUMGEN_H

// Singleton creating unique numbers unequal to zero
// just add NotZeroNumGen::getMsgNr()

#include <QtGlobal>

class NotZeroNumGen
{
public:
    static quint32 getMsgNr();
private:
    NotZeroNumGen() = default;
    quint32 getMsgNrFromObject();
    quint32 m_nMsgNr = 0;
    static NotZeroNumGen *m_theInstance;
};

#endif // NOTZERONUMGEN_H
