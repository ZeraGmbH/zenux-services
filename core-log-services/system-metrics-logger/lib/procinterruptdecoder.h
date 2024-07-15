#ifndef PROCINTERRUPTDECODER_H
#define PROCINTERRUPTDECODER_H

#include "interruptvalues.h"
#include <QString>

class ProcInterruptDecoder
{
public:
    static InterruptValues getCurrentInterrupts();
    static quint32 decodeSingleInterruptValue(QString infoLine);
private:
    static QString getInterruptInfo();
};

#endif // PROCINTERRUPTDECODER_H
