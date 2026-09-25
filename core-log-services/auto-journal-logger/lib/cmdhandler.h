#ifndef CMDHANDLER_H
#define CMDHANDLER_H

#include "logcreatorjournalsimple.h"
#include <QSimpleCmdHandlerBase>

class CmdHandler : public QSimpleCmdHandlerBase
{
    Q_OBJECT
public:
    explicit CmdHandler(const QString &coreFilePath, QObject *parent = nullptr);
    void StartCmd(SimpleCmdData *pCmd, QVariantList params) override;
private:
    bool makeDirWithParents(const QString &path);
    bool storeLogs(const QString &destinationDir);
    bool storeCoreDumps(const QString &destinationDir);
    bool storeVersionFile(const QString &destinationDir, const QString &versionFilePath);
    bool storeUpdateLogs(const QString &destinationDir);
    LogCreatorJournalSimple m_logGenerator;
    QString m_coreFilePath;
};

#endif // CMDHANDLER_H
