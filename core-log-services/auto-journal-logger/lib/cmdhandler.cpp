#include "cmdhandler.h"
#include "cmdparser.h"
#include "fileutils.h"
#include <QDir>
#include <QDateTime>

CmdHandler::CmdHandler(AbstractLogCreatorPtr logGenerator, QString coreFilePath, QObject *parent) :
    QSimpleCmdHandlerBase(parent),
    m_logGenerator(std::move(logGenerator)),
    m_coreFilePath(coreFilePath.endsWith("/") ? coreFilePath : coreFilePath + "/")
{
}

void CmdHandler::StartCmd(SimpleCmdData *pCmd, QVariantList params)
{
    QStringList errInfo;
    switch(static_cast<enum ControlCmdIds>(pCmd->GetCmdID())) {
    case CMD_SAVE_LOG_AND_DUMPS:
        QString hostName = FileUtils::getLocalHostname();
        if (hostName.isEmpty())
            hostName = "unknown-host";
        QDateTime now = QDateTime::currentDateTime();
        QString dateTime = now.toString("yyyy_MM_dd-HH_mm_ss");
        QString path = params[0].toString() + "/" + hostName + "/logs-and-dumps/" + dateTime;

        if(!makeDirWithParents(path))
            return;
        if(!storeLogs(path))
            return;
        if(!storeCoreDumps(path))
            return;
        QString versionFilePath = params[1].toString();
        if(!storeVersionFile(path, versionFilePath))
            return;

        emit OperationFinish(false, "");
        break;
    }
}

bool CmdHandler::makeDirWithParents(QString path)
{
    QDir dir;
    if(!dir.mkpath(path)) {
        emit OperationFinish(true, QStringLiteral("Could not create dir '%1'").arg(path));
        return false;
    }
    return true;
}

bool CmdHandler::storeLogs(QString path)
{
    if(!m_logGenerator->storeLogs(path)) {
        emit OperationFinish(true, QStringLiteral("Could not write journal to dir %1").arg(path));
        return false;
    }
    return true;
}

bool CmdHandler::storeCoreDumps(QString path)
{
    QDir coreDir(m_coreFilePath);
    QFileInfoList fileList = coreDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    for(auto &entry : fileList) {
        QString outputPath = path + "/" + entry.fileName();
        QString sourcePath = entry.absoluteFilePath();
        QString cmd = QString("mv %1 %2").arg(sourcePath, outputPath);
        if(system(qPrintable(cmd)) != 0) {
            emit OperationFinish(true, QStringLiteral("Could not move core file %1 to %2").arg(sourcePath, outputPath));
            return false;
        }
    }
    return true;
}

bool CmdHandler::storeVersionFile(QString path, QString versionFilePath)
{
    if(!versionFilePath.isEmpty()) {
        QString fileName = path + "/zenux-version.json";
        QString cmd = QString("mv %1 %2").arg(versionFilePath, fileName);
        if(system(qPrintable(cmd)) != 0) {
            emit OperationFinish(true, QStringLiteral("Could not move version file from %1 to %2").arg(versionFilePath, fileName));
            return false;
        }
    }
    return true;
}
