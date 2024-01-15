#include "atmelctrlfactorystatic.h"

std::function<AtmelCommonVersionsPtrS(QString, quint8, quint8, quint8, quint8)> AtmelCtrlFactoryStatic::m_emobCreateFunction =
    [](QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 ctrlChannel, quint8 debuglevel) {
    ZeraMcontrollerIoPtr i2cCtrl = std::make_shared<ZeraMControllerIo>(devnode, adrCtrl, debuglevel);
        return std::make_shared<AtmelCtrlEmob>(i2cCtrl, devnode, adrMux, ctrlChannel);
};

AtmelCommonVersionsPtrS AtmelCtrlFactoryStatic::getEmobCtrl(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 muxChannel, quint8 debuglevel)
{
    return m_emobCreateFunction(devnode, adrCtrl, adrMux, muxChannel, debuglevel);
}
