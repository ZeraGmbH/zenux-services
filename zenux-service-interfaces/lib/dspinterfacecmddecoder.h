#ifndef DSPINTERFACECMDDECODER_H
#define DSPINTERFACECMDDECODER_H

#include "dspmeasdata.h"

class DspInterfaceCmdDecoder
{
public:
    // Nightmare candidate: Make it go!!!
    static void setVarData(cDspMeasData *memgroup, QString datalist, DSPDATA::dType type = DSPDATA::dFloat);
};

#endif // DSPINTERFACECMDDECODER_H
