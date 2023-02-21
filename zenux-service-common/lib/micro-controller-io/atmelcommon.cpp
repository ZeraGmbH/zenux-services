#include "atmelcommon.h"

AtmelCommon::AtmelCommon(QString devnode, quint8 adr, quint8 debuglevel) :
    ZeraMcontrollerBase(devnode, adr, debuglevel)
{
}
