#include "atmelsyscntrltest.h"

AtmelSysCntrlTest::AtmelSysCntrlTest(QString devnode, quint8 adr, quint8 debuglevel) :
    cATMELSysCtrl(devnode, adr, debuglevel)
{
}

ZeraMControllerIo::atmelRM AtmelSysCntrlTest::readAccumulatorStatus(quint8 &stat)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIo::atmelRM AtmelSysCntrlTest::readAccumulatorSoc(quint8 &charge)
{
    charge = 37;
    return ZeraMControllerIo::atmelRM::cmddone;
}
