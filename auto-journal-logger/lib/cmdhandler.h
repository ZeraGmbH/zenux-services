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
    AbstractLogCreatorPtr m_logGenerator;
};

#endif // CMDHANDLER_H
