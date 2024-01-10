#ifndef ATMELSYSCNTRLTEST_H
#define ATMELSYSCNTRLTEST_H

#include "atmelctrlsystem.h"

class AtmelSysCntrlTest : public AtmelCtrlSystem
{
public:
    AtmelSysCntrlTest(QString devnode, quint8 adr, quint8 debuglevel);
    atmelRM readAccumulatorStatus(quint8& stat) override;
    atmelRM readAccuStateOfCharge(quint8& charge) override;
};

#endif // ATMELSYSCNTRLTEST_H
