#ifndef ATMELCTRLSSYSTEM_H
#define ATMELCTRLSSYSTEM_H

#include "abstracti2ccontrollers.h"
#include <intelhexfileio.h>
#include <zeramcontrollerio.h>
#include <QString>

class AtmelCtrlSystem : public ZeraMControllerIo, public AbstractI2cCtrlAccumulator
{
public:
    AtmelCtrlSystem(QString devnode, quint8 adr, quint8 debuglevel);

    atmelRM readAccumulatorStatus(quint8& stat) override;
    atmelRM readAccuStateOfCharge(quint8& charge) override;
    atmelRM enableTestMode(qint32 testBits) override;
};

#endif // ATMELCTRLSSYSTEM_H
