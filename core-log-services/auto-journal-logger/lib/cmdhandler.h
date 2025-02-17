#ifndef CMDHANDLER_H
#define CMDHANDLER_H

#include "abstractlogcreator.h"
#include <QSimpleCmdHandlerBase>

class CmdHandler : public QSimpleCmdHandlerBase
{
    Q_OBJECT
public:
    explicit CmdHandler(AbstractLogCreatorPtr logGenerator, QString coreFilePath, QObject *parent = nullptr);
    void StartCmd(SimpleCmdData *pCmd, QVariantList params) override;
private:
    bool makeDirWithParents(QString path);
    bool storeLogs(QString destinationDir);
    bool storeCoreDumps(QString destinationDir);
    bool storeVersionFile(QString destinationDir, QString versionFilePath);
    bool storeUpdateLogs(QString destinationDir, QString updateLogsSourceDir);
    AbstractLogCreatorPtr m_logGenerator;
    QString m_coreFilePath;
};

#endif // CMDHANDLER_H
