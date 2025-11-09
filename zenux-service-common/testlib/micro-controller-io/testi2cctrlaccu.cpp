#include "testi2cctrlaccu.h"
#include "controllerpersitentdata.h"

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlAccu::readAccuStatus(quint8 &stat)
{
    stat = ControllerPersitentData::getData().m_accuStatus;
    return ZeraMControllerIoTemplate::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlAccu::readAccuStateOfCharge(quint8 &charge)
{
    charge = ControllerPersitentData::getData().m_accuCharge;
    return ZeraMControllerIoTemplate::cmddone;
}
