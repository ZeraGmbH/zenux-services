#ifndef CMDHANDLER_H
#define CMDHANDLER_H

#include <QSimpleCmdHandlerBase>

class PeriodicLoggerWrapper;

class CmdHandler : public QSimpleCmdHandlerBase
{
    Q_OBJECT
public:
    explicit CmdHandler(PeriodicLoggerWrapper* server, QObject *parent = nullptr);
    void StartCmd(SimpleCmdData *pCmd, QVariantList params) override;
private:
    PeriodicLoggerWrapper* m_logger;
};

#endif // CMDHANDLER_H
