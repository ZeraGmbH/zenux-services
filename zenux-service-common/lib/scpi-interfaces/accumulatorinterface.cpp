#include "accumulatorinterface.h"
#include "timerfactoryqt.h"

constexpr quint8 ERROR = 0x80;
constexpr int ACCU_POLLING_PERIOD_MS = 1000;

enum accumulatorCommands {
    cmdStatus,
    cmdAccuStateOfCharge
};

AccumulatorInterface::AccumulatorInterface(std::shared_ptr<cSCPI> scpiInterface,
                                           AccumulatorSettings *settings,
                                           AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiServerConnection(scpiInterface),
    m_ctrlFactory(ctrlFactory)
{
    if(settings->isAvailable()) {
        m_pollingTimer = TimerFactoryQt::createPeriodic(ACCU_POLLING_PERIOD_MS);
        connect(m_pollingTimer.get(), &TimerTemplateQt::sigExpired, this, &AccumulatorInterface::getAccuStateOfCharge);
        connect(m_pollingTimer.get(), &TimerTemplateQt::sigExpired, this, &AccumulatorInterface::getAccumulatorStatus);
        m_pollingTimer->start();
    }
}

void AccumulatorInterface::initSCPIConnection()
{
    addDelegate("SYSTEM:ACCUMULATOR", "STATUS", SCPI::isQuery, m_scpiInterface, accumulatorCommands::cmdStatus, &m_accumulatorStatus);
    addDelegate("SYSTEM:ACCUMULATOR", "SOC", SCPI::isQuery, m_scpiInterface, accumulatorCommands::cmdAccuStateOfCharge, &m_accuStateOfCharge);
}

void AccumulatorInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode)
    {
    case accumulatorCommands::cmdStatus:
        protoCmd->m_sOutput = m_accumulatorStatus.getString();
        break;
    case accumulatorCommands::cmdAccuStateOfCharge:
        protoCmd->m_sOutput = m_accuStateOfCharge.getString();
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

void AccumulatorInterface::getAccumulatorStatus()
{
    quint8 status = 0;
    if(m_ctrlFactory->getAccuController()->readAccuStatus(status) == ZeraMControllerIo::atmelRM::cmddone) {
        m_accumulatorStatus = QString::number(status);
        QString newStatus = m_accumulatorStatus.getString();
        if(m_oldaccuStatus != newStatus) {
            m_oldaccuStatus = newStatus;
            emit sigAccumulatorStatusChange(status);
        }
    }
    else
        m_accumulatorStatus = QString::number(ERROR);
}

void AccumulatorInterface::getAccuStateOfCharge()
{
    quint8 charge = 0;
    if(m_ctrlFactory->getAccuController()->readAccuStateOfCharge(charge) == ZeraMControllerIo::atmelRM::cmddone)
        m_accuStateOfCharge = QString::number(charge);
    else
        m_accuStateOfCharge = QString::number(ERROR);
}
