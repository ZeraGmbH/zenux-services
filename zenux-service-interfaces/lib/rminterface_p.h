#ifndef RMINTERFACE_P_H
#define RMINTERFACE_P_H

#include <QObject>
#include <QStringList>

#include "interface_p.h"
#include "rminterface.h"
#include "dspvargroupclientinterface.h"

namespace Zera {

enum rmcommands
{
    rmident,
    rmscpi,
    addresource,
    removeresource,
    getresourcetypes,
    getresources,
    getresourceinfo,
    setresource,
    freeresource
};


class cRMInterface;

class cRMInterfacePrivate: public cInterfacePrivate
{
    Q_OBJECT
public:
    explicit cRMInterfacePrivate(cRMInterface* iface);
    void setClientSmart(Zera::ProxyClientPtr client);
    quint32 scpiCommand(const QString &scpi);

    quint32 rmIdent(const QString &name);
    quint32 addResource(const QString &type, const QString &name, int n, const QString &description, quint16 port);
    quint32 removeResource(const QString &type, const QString &name);
    quint32 getResourceTypes();
    quint32 getResources(const QString &type);
    quint32 getResourceInfo(const QString &type, const QString &name);
    quint32 setResource(const QString &type, const QString &name, int n);
    quint32 freeResource(const QString &type, const QString &name);

protected slots:
    void receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message) override;
    void receiveError(QAbstractSocket::SocketError errorCode) override;

private:
    Q_DECLARE_PUBLIC(cRMInterface)
    cRMInterface *q_ptr;

    QStringList CycCmdList, IntCmdList;
    QList<DspVarGroupClientInterface*> m_DspMeasDataList; // eine liste mit zeigern auf "programmdaten"
    QList<DspVarGroupClientInterface*> m_DspMemoryDataList; // eine liste mit zeigern auf  dsp speicher allgemein

};

}

#endif // RMINTERFACE_P_H
