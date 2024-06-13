#include "cmdparser.h"
#include "periodicloggerwrapper.h"
#include "fileutils.h"

CmdParser::CmdParser(PeriodicLoggerWrapper *logger, QObject *parent) :
    QSimpleCmdParserSocketBase(parent),
    m_logger(logger)
{
    AddCmdInfo("StartLog",
               CmdParamTypeIdList() << PARAM_TYPE_STRING << PARAM_TYPE_INT,
               CMD_START_PERIODIC_LOG,
               true,
               QStringLiteral("Parameter:\nLogFileNameFull\nLogFrequency seconds [10;1000]\nReturn: Log status info"));
    AddCmdInfo("StopLog",
               CmdParamTypeIdList(),
               CMD_STOP_PERIODIC_LOG,
               true);
}

QString CmdParser::GetParserName()
{
    return QStringLiteral("PeriodicLogControl");
}

const QString CmdParser::PlausiCheck(SimpleCmdData *pCmd, const QVariantList &params)
{
    QStringList errInfo;
    switch(pCmd->GetCmdID())
    {
    case CMD_START_PERIODIC_LOG:
        if(m_logger->isLogActive())
            errInfo.append(QStringLiteral("Logger is already running"));
        if(!FileUtils::dirExists(params[0].toString()))
            errInfo.append(QStringLiteral("Parameter LogFileNameFull %1 is invalid - dir exists???").arg(params[0].toString()));
        if(params[1].toInt() < 10 || params[1].toInt() > 1000)
            errInfo.append(QStringLiteral("Parameter LogFrequency out of limits: %1 allowed [10;1000]").arg(params[1].toInt()));
        break;
    case CMD_STOP_PERIODIC_LOG:
        break;
    }
    QString strRet;
    if(!errInfo.isEmpty())
        strRet = FormatErrorMsg(pCmd->GetDisplayStr(), errInfo.join("\n"));
    return strRet;
}
