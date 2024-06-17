#include "cmdparser.h"
#include "fileutils.h"
#include <QDir>

CmdParser::CmdParser(QObject *parent) :
    QSimpleCmdParserSocketBase(parent)
{
    AddCmdInfo("SaveLogAndDumps",
               CmdParamTypeIdList() << PARAM_TYPE_STRING,
               CMD_SAVE_LOG_AND_DUMPS,
               true,
               QStringLiteral("Parameter: DestDir\nReturn: Log status info"));
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
    case CMD_SAVE_LOG_AND_DUMPS:
        QString path = params[0].toString();
        QString hostName = FileUtils::getLocalHostname();
        if(hostName.isEmpty())
            errInfo.append(QStringLiteral("Cannot read local hostname!"));
        if(path.isEmpty())
            errInfo.append(QStringLiteral("Parameter DestDir cannot be empty!").arg(params[0].toString()));
        else {
            QString parentPath = FileUtils::getExistingParentNonRootPath(params[0].toString());
            if(path != parentPath)
                errInfo.append(QStringLiteral("Parameter DestDir '%1' invalid!").arg(params[0].toString()));
        }
        break;
    }
    QString strRet;
    if(!errInfo.isEmpty())
        strRet = FormatErrorMsg(pCmd->GetDisplayStr(), errInfo.join("\n"));
    return strRet;
}
