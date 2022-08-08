#ifndef ATMELERRORLOG_H
#define ATMELERRORLOG_H

#include <QtGlobal>

class AtmelErrorLog
{
public:
    static void logReadRange(quint8 channel, quint32 errorMask);
    static void logSetRange(quint8 channel, quint8 range, quint32 errorMask);
};

#endif // ATMELERRORLOG_H
