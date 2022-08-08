#include "atmelerrorlog.h"

void AtmelErrorLog::logReadRange(quint8 channel, quint32 errorMask)
{
    if(errorMask != 0) {
        qWarning("ReadRange failed with ch: %i / mask: %8X failed!",
                 channel, errorMask);
    }
}

void AtmelErrorLog::logSetRange(quint8 channel, quint8 range, quint32 errorMask)
{
    if(errorMask != 0) {
        qWarning("SetRange failed with ch: %i / rng: %i / mask: %8X failed!",
                 channel, range, errorMask);
    }
}
