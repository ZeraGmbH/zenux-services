#include "cmdhandler.h"
#include "cmdparser.h"
#include "fileutils.h"
#include <QDir>

CmdHandler::CmdHandler(AbstractLogCreatorPtr logGenerator, QObject *parent) :
    QSimpleCmdHandlerBase(parent),
    m_logGenerator(std::move(logGenerator))
{
}

void CmdHandler::StartCmd(SimpleCmdData *pCmd, QVariantList params)
{
    QStringList errInfo;
    switch(static_cast<enum ControlCmdIds>(pCmd->GetCmdID())) {
    case CMD_SAVE_LOG_AND_DUMPS:
        QString hostName = FileUtils::getLocalHostname();
        QString path = params[0].toString() + "/" + hostName + "/logs-and-dumps";
        QDir dir;
        if(!dir.mkpath(path)) {
            emit OperationFinish(true, QStringLiteral("Could not create dir '%1'").arg(path));
            return;
        }
        if(!m_logGenerator->storeLogs(path))
            emit OperationFinish(true, QStringLiteral("Could not write journal").arg(path));
        emit OperationFinish(false, "");
        break;
    }
}
