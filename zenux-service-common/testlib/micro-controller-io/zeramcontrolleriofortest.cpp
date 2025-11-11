#include "zeramcontrolleriofortest.h"

ZeraMcontrollerIoForTest::ZeraMcontrollerIoForTest(QString devnode, quint8 adr, quint8 debuglevel) :
    ZeraMControllerIoTemplate(devnode, adr, debuglevel)
{
}

void ZeraMcontrollerIoForTest::simulateApplicationRunnung()
{
    m_return = cmdfault;
}

void ZeraMcontrollerIoForTest::simulateBooloaderRunning()
{
    m_return = cmddone;
}

ZeraMControllerIoTemplate::atmelRM ZeraMcontrollerIoForTest::bootloaderStartProgram()
{
    return m_return;
}

ZeraMControllerIoTemplate::atmelRM ZeraMcontrollerIoForTest::readVariableLenText(quint16 hwcmd, QString &answer, quint8 *extraParam, quint16 extraParamLen)
{
    Q_UNUSED(extraParam)
    Q_UNUSED(extraParamLen)
    answer = QString("%i").arg(hwcmd);
    return cmddone;
}
