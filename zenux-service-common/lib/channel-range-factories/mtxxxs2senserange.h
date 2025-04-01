#ifndef MT310s2SENSERANGE_H
#define MT310s2SENSERANGE_H

#include "senserangecommon.h"

class MtXXXs2SenseRange: public SenseRangeCommon
{
    Q_OBJECT
public:
    MtXXXs2SenseRange(std::shared_ptr<cSCPI> scpiinterface,
                      QString name,
                      bool avail,
                      double rValue,
                      double rejection,
                      double ovrejection,
                      quint8 rselcode,
                      quint16 mmask);
};

#endif // MT310s2SENSERANGE_H
