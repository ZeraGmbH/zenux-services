#ifndef CLAMPSENSERANGE_H
#define CLAMPSENSERANGE_H

#include "senserangecommon.h"

class ClampSenseRange: public SenseRangeCommon
{
    Q_OBJECT
public:
    ClampSenseRange(const std::shared_ptr<cSCPI> &scpiInterface,
                    const QString &name,
                    bool avail,
                    double rValue,
                    double rejection,
                    double ovrejection,
                    quint8 rselcode,
                    quint16 mmask,
                    AdjRangeScpi* justdata);
};

#endif // CLAMPSENSERANGE_H
