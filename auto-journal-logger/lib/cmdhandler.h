#ifndef CMDHANDLER_H
#define CMDHANDLER_H

#include "abstractlogcreator.h"
#include <QSimpleCmdHandlerBase>

class CmdHandler : public QSimpleCmdHandlerBase
{
    Q_OBJECT
public:
    explicit CmdHandler(AbstractLogCreatorPtr logGenerator, QObject *parent = nullptr);
    void StartCmd(SimpleCmdData *pCmd, QVariantList params) override;
private:
    bool makeDirWithParents(QString path);
    bool storeLogs(QString path);
    bool storeCoreDumps(QString path);
    bool storeVersionFile(QString path, QString versionFilePath);
    AbstractLogCreatorPtr m_logGenerator;
};

#endif // CMDHANDLER_H
