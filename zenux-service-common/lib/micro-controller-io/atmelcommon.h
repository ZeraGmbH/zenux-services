#ifndef ATMELCOMMON_H
#define ATMELCOMMON_H

#include <zera_mcontroller_base.h>

class AtmelCommon : public ZeraMcontrollerBase
{
public:
    AtmelCommon(QString devnode, quint8 adr, quint8 debuglevel);
    virtual ~AtmelCommon() = default;

    atmelRM writeIntMask(quint16 mask);
    atmelRM readIntMask(quint16& mask);
    atmelRM readCriticalStatus(quint16& stat);
    atmelRM resetCriticalStatus(quint16 stat);
};

#endif // ATMELCOMMON_H
