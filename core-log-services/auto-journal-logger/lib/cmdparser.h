#ifndef CMDPARSER_H
#define CMDPARSER_H

#include <QSimpleCmdParserSocketBase>

enum ControlCmdIds
{
    CMD_SAVE_LOG_AND_DUMPS,
};

class CmdParser : public QSimpleCmdParserSocketBase
{
    Q_OBJECT
public:
    explicit CmdParser(QObject *parent = nullptr);
    QString GetParserName() override;
private:
    const QString PlausiCheck(SimpleCmdData *pCmd, const QVariantList &params) override;
};

#endif // CMDPARSER_H
