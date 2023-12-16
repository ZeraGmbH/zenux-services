#ifndef SENSERANGETESTTEMPLATE_H
#define SENSERANGETESTTEMPLATE_H
#include "senserange.h"
#include "senseinterface.h"

#include <scpi.h>

class SenseRangeTestTemplate
{
protected:
    void _init(QString leadingNodes, QString alias, double rValue, double rejection, double ovrejection, double adcrejection, quint16 mmask);
    void _cleanup();

    cSCPI *scpi;
    JustRangeTripletOffsetGainPhaseMt310s2 *justData;
    cSenseRange *testRange;
};

#endif // SENSERANGETESTTEMPLATE_H
