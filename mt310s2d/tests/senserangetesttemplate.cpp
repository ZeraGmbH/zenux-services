#include "senserangetesttemplate.h"


void SenseRangeTestTemplate::_init()
{
    scpi = new cSCPI("foo");
    justData = new cMT310S2JustData(scpi);
    testRange = new cSenseRange(scpi, "100mA", "100mA", true, 1000, 1000, 1000, 1000, 0, SenseSystem::modeAC, justData);
}

void SenseRangeTestTemplate::_cleanup()
{
    delete testRange;
    delete scpi;
}
