#ifndef SENSERANGE_H
#define SENSERANGE_H

#include "senserangecommon.h"

class Com5003SenseRange : public SenseRangeCommon
{
    Q_OBJECT
public:
    Com5003SenseRange(cSCPI *scpiinterface,
                QString name,
                bool avail,
                double rValue,
                double rejection,
                double ovrejection,
                quint8 rselcode);
};

#endif // SENSERANGE_H
