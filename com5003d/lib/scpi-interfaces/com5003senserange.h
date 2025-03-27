#ifndef COM5003SENSERANGE_H
#define COM5003SENSERANGE_H

#include "senserangecommon.h"

class Com5003SenseRange : public SenseRangeCommon
{
    Q_OBJECT
public:
    Com5003SenseRange(std::shared_ptr<cSCPI> scpiinterface,
                      QString name,
                      bool avail,
                      double rValue,
                      double rejection,
                      double ovrejection,
                      quint8 rselcode,
                      quint16 mmask);
};

#endif // COM5003SENSERANGE_H
