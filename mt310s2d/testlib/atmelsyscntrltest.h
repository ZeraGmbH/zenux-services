#ifndef ATMELSYSCNTRLTEST_H
#define ATMELSYSCNTRLTEST_H

#include "atmelsysctrl.h"

class AtmelSysCntrlTest : public cATMELSysCtrl
{
public:
    AtmelSysCntrlTest(QString devnode, quint8 adr, quint8 debuglevel);
    atmelRM readAccumulatorStatus(quint16& stat) override;
};

#endif // ATMELSYSCNTRLTEST_H
