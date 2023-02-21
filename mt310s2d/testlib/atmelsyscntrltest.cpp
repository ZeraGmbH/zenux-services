#include "atmelsyscntrltest.h"

AtmelSysCntrlTest::AtmelSysCntrlTest(QString devnode, quint8 adr, quint8 debuglevel) :
    cATMELSysCtrl(devnode, adr, debuglevel)
{
}

ZeraMcontrollerBase::atmelRM AtmelSysCntrlTest::readAccumulatorStatus(quint16 &stat)
{
    return ZeraMcontrollerBase::atmelRM::cmddone;
}
