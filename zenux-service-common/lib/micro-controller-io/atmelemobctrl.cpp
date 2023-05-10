#include "atmelemobctrl.h"

AtmelEmobCtrl::AtmelEmobCtrl(QString devnode, quint8 adr, quint8 ctrlChannelForMux, quint8 debuglevel) :
    ZeraMcontrollerBase(devnode, adr, debuglevel)
{
}

ZeraMcontrollerBase::atmelRM AtmelEmobCtrl::readCTRLVersion(QString &answer)
{

}

ZeraMcontrollerBase::atmelRM AtmelEmobCtrl::readPCBVersion(QString &answer)
{

}
