#ifndef RESOURCEREGISTERTRANSACTION_H
#define RESOURCEREGISTERTRANSACTION_H

#include "rmconnection.h"
#include <QObject>

class ResourceRegisterTransaction : public QObject
{
    Q_OBJECT
public:
    ResourceRegisterTransaction(RMConnection *rmConnection);
    void register1Resource(QString registerParameter);
signals:
    void registerRdy();
private slots:
    void resourceManagerAck(quint32 msgnr);
private:
    RMConnection *m_rmConnection;
    QList<quint32> m_msgNrListRegister;
};

#endif // RESOURCEREGISTERTRANSACTION_H
