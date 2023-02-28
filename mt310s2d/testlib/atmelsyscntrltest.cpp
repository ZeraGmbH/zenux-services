#include "atmelsyscntrltest.h"

AtmelSysCntrlTest::AtmelSysCntrlTest(QString devnode, quint8 adr, quint8 debuglevel) :
    cATMELSysCtrl(devnode, adr, debuglevel)
{
}

ZeraMcontrollerBase::atmelRM AtmelSysCntrlTest::readAccumulatorStatus(quint8 &stat)
{
    return ZeraMcontrollerBase::atmelRM::cmddone;
}

ZeraMcontrollerBase::atmelRM AtmelSysCntrlTest::readAccumulatorSoc(quint8 &charge)
{
    charge = 37;
    return ZeraMcontrollerBase::atmelRM::cmddone;
}
