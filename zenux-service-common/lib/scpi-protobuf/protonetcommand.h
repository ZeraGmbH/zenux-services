#ifndef PROTONETCOMMAND_H
#define PROTONETCOMMAND_H

#include <vtcp_peer.h>
#include <QByteArray>
#include <QString>
#include <memory>

class ProtonetCommand
{
public:
    ProtonetCommand(VeinTcp::TcpPeer* peer,
                    bool hasClientId,
                    bool withOutput,
                    const QByteArray &clientid,
                    quint32 messagenr,
                    const QString &input,
                    quint8 scpiType = 0);
    ProtonetCommand(const ProtonetCommand& other);
    virtual ~ProtonetCommand();
    static int getInstanceCount();

    VeinTcp::TcpPeer* m_pPeer;
    bool m_bhasClientId;
    bool m_bwithOutput;
    QByteArray m_clientId;
    quint32 m_nmessageNr;
    QString m_sInput;
    QString m_sOutput;
    quint8 m_nSCPIType = 0; // just for SEC - we have to get rid!!
private:
    static int m_instanceCount;
};

typedef std::shared_ptr<ProtonetCommand> ProtonetCommandPtr;
Q_DECLARE_METATYPE(ProtonetCommandPtr)

#endif // PROTONETCOMMAND_H
