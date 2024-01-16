#ifndef ATMELCTRLSSYSTEM_H
#define ATMELCTRLSSYSTEM_H

#include "atmelbaseimplementation.h"
#include <intelhexfileio.h>
#include <zeramcontrollerio.h>
#include <QString>

class AtmelCtrlSystem : public AtmelBaseImplementation, public AtmelCommonVersions, public AtmelAccumulatorHandler
{
public:
    AtmelCtrlSystem(QString devnode, quint8 adr, quint8 debuglevel);

    atmelRM readCTRLVersion(QString& answer) override;
    atmelRM readPCBVersion(QString& answer) override;
    atmelRM readAccumulatorStatus(quint8& stat) override;
    atmelRM readAccuStateOfCharge(quint8& charge) override;
    atmelRM enableTestMode(qint32 testBits) override;
};

#endif // ATMELCTRLSSYSTEM_H
