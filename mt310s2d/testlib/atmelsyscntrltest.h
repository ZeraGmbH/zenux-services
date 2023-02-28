#ifndef ATMELSYSCNTRLTEST_H
#define ATMELSYSCNTRLTEST_H

#include "atmelsysctrl.h"

class AtmelSysCntrlTest : public cATMELSysCtrl
{
public:
    AtmelSysCntrlTest(QString devnode, quint8 adr, quint8 debuglevel);
    atmelRM readAccumulatorStatus(quint8& stat) override;
    atmelRM readAccumulatorSoc(quint8& charge) override;
};

#endif // ATMELSYSCNTRLTEST_H
