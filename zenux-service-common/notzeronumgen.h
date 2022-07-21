#ifndef NOTZERONUMGEN_H
#define NOTZERONUMGEN_H

#include <QtGlobal>

class NotZeroNumGen
{
public:
    quint32 getMsgNr();
private:
    quint32 m_nMsgNr = 0;
};

#endif // NOTZERONUMGEN_H
