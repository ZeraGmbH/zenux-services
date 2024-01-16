#ifndef ATMELBASEIMPLEMENTATION_H
#define ATMELBASEIMPLEMENTATION_H

#include "abstracti2ccontrollers.h"

class AtmelBaseImplementation : public ZeraMControllerIo, public AtmelCriticalStatus
{
public:
    AtmelBaseImplementation(QString devnode, quint8 adr, quint8 debuglevel);
    virtual ~AtmelBaseImplementation() = default;

    atmelRM writeIntMask(quint16 mask) override;
    atmelRM readIntMask(quint16& mask) override;
    atmelRM readCriticalStatus(quint16& stat) override;
    atmelRM resetCriticalStatus(quint16 stat) override;
};

#endif // ATMELBASEIMPLEMENTATION_H
