#include "senserangetesttemplate.h"


void SenseRangeTestTemplate::_init(QString leadingNodes, QString alias, double rValue, double rejection, double ovrejection, double adcrejection, quint16 mmask)
{
    scpi = new cSCPI("foo");
    justData = new cMT310S2JustData(scpi);
    testRange = new cSenseRange(scpi, alias, alias, true, rValue, rejection, ovrejection, adcrejection, 0, mmask, justData);
    testRange->initSCPIConnection(leadingNodes);
}

void SenseRangeTestTemplate::_cleanup()
{
    delete testRange;
    delete scpi;
}
