#include "accumulatorinterface.h"
#include "timerfactoryqt.h"

constexpr quint8 ERROR = 0x80;
constexpr int ACCU_POLLING_PERIOD_MS = 1000;

AccumulatorInterface::AccumulatorInterface(cSCPI *scpiInterface, std::shared_ptr<AtmelCtrlSystem> systemController, AccumulatorSettings *settings) :
    ScpiConnection(scpiInterface),
    m_systemController(systemController)
{
    m_pollingTimer = TimerFactoryQt::createPeriodic(ACCU_POLLING_PERIOD_MS);
    connect(m_pollingTimer.get(), &TimerTemplateQt::sigExpired, this, &AccumulatorInterface::getAccuStateOfCharge);
    connect(m_pollingTimer.get(), &TimerTemplateQt::sigExpired, this, &AccumulatorInterface::getAccumulatorStatus);
    if(settings->isAvailable())
        m_pollingTimer->start();
}

void AccumulatorInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:ACCUMULATOR").arg(leadingNodes),"STATUS",SCPI::isQuery, m_pSCPIInterface, accumulatorCommands::cmdStatus, &m_accumulatorStatus);
    addDelegate(QString("%1SYSTEM:ACCUMULATOR").arg(leadingNodes),"SOC",SCPI::isQuery, m_pSCPIInterface, accumulatorCommands::cmdAccuStateOfCharge, &m_accuStateOfCharge);
}

void AccumulatorInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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

QString AccumulatorInterface::getAccumulatorStatus()
{
    quint8 status = 0;
    if(m_systemController->readAccumulatorStatus(status) == ZeraMControllerIo::atmelRM::cmddone){
        m_accumulatorStatus = QString::number(status);
    }
    else{
        m_accumulatorStatus = QString::number(ERROR);
    }
    return m_accumulatorStatus.getString();
}

QString AccumulatorInterface::getAccuStateOfCharge()
{
    quint8 charge = 0;
    if(m_systemController->readAccuStateOfCharge(charge) == ZeraMControllerIo::atmelRM::cmddone){
        m_accuStateOfCharge = QString::number(charge);
    }
    else{
        m_accuStateOfCharge = QString::number(ERROR);
    }
    return m_accuStateOfCharge.getString();
}
