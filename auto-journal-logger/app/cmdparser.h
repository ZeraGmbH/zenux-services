#ifndef CMDPARSER_H
#define CMDPARSER_H

#include <QSimpleCmdParserSocketBase>

enum ControlCmdIds
{
    CMD_START_PERIODIC_LOG,
    CMD_STOP_PERIODIC_LOG,
};

class PeriodicLoggerWrapper;

class CmdParser : public QSimpleCmdParserSocketBase
{
    Q_OBJECT
public:
    explicit CmdParser(PeriodicLoggerWrapper* logger, QObject *parent = nullptr);
    QString GetParserName() override;
private:
    const QString PlausiCheck(SimpleCmdData *pCmd, const QVariantList &params) override;
    PeriodicLoggerWrapper *m_logger;
};

#endif // CMDPARSER_H
