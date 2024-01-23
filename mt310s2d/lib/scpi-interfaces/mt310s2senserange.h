#ifndef SENSERANGE_H
#define SENSERANGE_H

#include "senserangecommon.h"

class Mt310s2SenseRange: public SenseRangeCommon
{
    Q_OBJECT
public:
    Mt310s2SenseRange(cSCPI* scpiinterface,
                      QString name,
                      bool avail,
                      double rValue,
                      double rejection,
                      double ovrejection,
                      quint8 rselcode,
                      quint16 mmask,
                      RangeAdjInterface* justdata);
    void setMModeToUpdateAvailability(int mode);
};

#endif // SENSERANGE_H
