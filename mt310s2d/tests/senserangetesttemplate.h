#ifndef SENSERANGETESTTEMPLATE_H
#define SENSERANGETESTTEMPLATE_H
#include "senserange.h"
#include "senseinterface.h"

#include <scpi.h>

class SenseRangeTestTemplate
{
protected:
    void _init();
    void _cleanup();

    cSCPI *scpi;
    cMT310S2JustData *justData;
    cSenseRange *testRange;
};

#endif // SENSERANGETESTTEMPLATE_H
