#include "statusinterface.h"
#include "protonetcommand.h"
#include <timerfactoryqt.h>
#include "zscpi_response_definitions.h"
#include <scpi.h>

constexpr int AUTH_POLLING_PERIOD_MS = 1000;

enum StatusCommands
{
    cmdDevice,
    cmdAdjustment,
    cmdAuthorization
};

cStatusInterface::cStatusInterface(cSCPI *scpiInterface, AdjustmentStatusInterface *adjustmentStatusInterface, AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiConnection(scpiInterface),
    m_adjustmentStatusInterface(adjustmentStatusInterface),
    m_ctrlFactory(ctrlFactory)
{
    m_periodicTimer = TimerFactoryQt::createPeriodic(AUTH_POLLING_PERIOD_MS);
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired, this, &cStatusInterface::getAuthorizationStatus);
}

void cStatusInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1STATUS").arg(leadingNodes),"DEVICE",SCPI::isQuery, m_pSCPIInterface, cmdDevice);
    addDelegate(QString("%1STATUS").arg(leadingNodes),"ADJUSTMENT", SCPI::isQuery, m_pSCPIInterface, cmdAdjustment);
    addDelegate(QString("%1STATUS").arg(leadingNodes),"AUTHORIZATION", SCPI::isQuery, m_pSCPIInterface, cmdAuthorization, &m_notifierAutorization);
    connect(this, &ScpiConnection::removingSubscribers, this, &cStatusInterface::onNotifierUnregistered);
}

void cStatusInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isQuery()) {
        switch (cmdCode)
        {
        case cmdDevice:
            protoCmd->m_sOutput = getControllerAvail();
            break;
        case cmdAdjustment:
            protoCmd->m_sOutput = QString("%1").arg(m_adjustmentStatusInterface->getAdjustmentStatus());
            break;
        case cmdAuthorization:
            protoCmd->m_sOutput = getAuthorizationStatus();
            break;
        }
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString cStatusInterface::getControllerAvail()
{
    bool dummy;
    if (m_ctrlFactory->getPermissionCheckController()->hasPermission(dummy)) // no problem reading from atmel
        return "1"; // means device available
    else
        return "0";
}

QString cStatusInterface::getAuthorizationStatus()
{
    QString status = "0";
    bool enable;
    if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable) && enable)
        status = "1";
    m_notifierAutorization = status;
    return status;
}

void cStatusInterface::onNotifierRegistered(NotificationString *notifier)
{
    if(&m_notifierAutorization==notifier)
        m_periodicTimer->start();
}

void cStatusInterface::onNotifierUnregistered()
{
    m_periodicTimer->stop();
}

