#include "cmdhandler.h"
#include "cmdparser.h"
#include "periodicloggerwrapper.h"

CmdHandler::CmdHandler(PeriodicLoggerWrapper *server, QObject *parent) :
    QSimpleCmdHandlerBase(parent),
    m_logger(server)
{
}

void CmdHandler::StartCmd(SimpleCmdData *pCmd, QVariantList params)
{
    switch(static_cast<enum ControlCmdIds>(pCmd->GetCmdID())) {
    case CMD_START_PERIODIC_LOG:
        m_logger->startPeriodicLog(params[0].toString(), params[1].toInt());
        emit OperationFinish(false, QStringLiteral("Periodic log active"));
        break;
    case CMD_STOP_PERIODIC_LOG:
        if(!m_logger->isLogActive())
            emit OperationFinish(false, QStringLiteral("Log server is not running - ignore stop request"));
        else {
            m_logger->stopPeriodicLog();
            emit OperationFinish(false, QStringLiteral("Periodic log stopped"));
        }
        break;
    }
}
