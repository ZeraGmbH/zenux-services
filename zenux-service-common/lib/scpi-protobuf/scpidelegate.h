#ifndef SCPIDELEGATE_H
#define SCPIDELEGATE_H

#include "scpiobject.h"
#include "protonetcommand.h"
#include <scpi.h>
#include <QObject>
#include <QString>

class cSCPIDelegate: public QObject, public cSCPIObject
{
   Q_OBJECT
public:
    cSCPIDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode);
    virtual bool executeSCPI(const QString&, QString&) override { return false; }
    virtual bool executeSCPI(cProtonetCommand* protoCmd);
    QString getCommand();
signals:
    void sigExecuteProtoScpi(int cmdCode, cProtonetCommand* protoCmd);
private:
    quint16 m_nCmdCode;
    QString m_sCommand;
};

#endif // SCPIDELEGATE_H
