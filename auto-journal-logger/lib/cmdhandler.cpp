#include "cmdhandler.h"
#include "cmdparser.h"
#include "fileutils.h"
#include <QDir>
#include <QDateTime>

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
        QDateTime now = QDateTime::currentDateTime();
        QString dateTime = now.toString("yyyy_MM_dd-HH_mm_ss");
        QString path = params[0].toString() + "/" + hostName + "/logs-and-dumps/" + dateTime;
        QDir dir;
        if(!dir.mkpath(path)) {
            emit OperationFinish(true, QStringLiteral("Could not create dir '%1'").arg(path));
            return;
        }
        if(!m_logGenerator->storeLogs(path)) {
            emit OperationFinish(true, QStringLiteral("Could not write journal").arg(path));
            return;
        }
        QString versionFilePath = params[1].toString();
        if(!versionFilePath.isEmpty()) {
            QFile versionFile(versionFilePath);
            if(!versionFile.exists()) {
                emit OperationFinish(true, QStringLiteral("Version file %1 does not exist").arg(versionFilePath));
                return;
            }
            QString fileName = path + "/zenux-version.json";
            versionFile.copy(fileName);
        }
        emit OperationFinish(false, "");
        break;
    }
}
