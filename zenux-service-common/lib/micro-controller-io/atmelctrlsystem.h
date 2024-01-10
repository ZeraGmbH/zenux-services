#ifndef ATMELCTRLSSYSTEM_H
#define ATMELCTRLSSYSTEM_H

#include <atmelcommon.h>
#include <atmelcommonversions.h>
#include <intelhexfileio.h>
#include <zeramcontrollerio.h>
#include <QString>

class AtmelCtrlSystem : public AtmelCommon, public AtmelCommonVersions
{
public:
    AtmelCtrlSystem(QString devnode, quint8 adr, quint8 debuglevel);

    atmelRM readCTRLVersion(QString& answer) override;
    atmelRM readPCBVersion(QString& answer) override;
    virtual atmelRM readAccumulatorStatus(quint8& stat);
    virtual atmelRM readAccuStateOfCharge(quint8& charge);
    atmelRM enableTestMode(qint32 testBits);
};

#endif // ATMELCTRLSSYSTEM_H
