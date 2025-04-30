#include "protonetcommand.h"

int ProtonetCommand::m_instanceCount = 0;

ProtonetCommand::ProtonetCommand(VeinTcp::TcpPeer *peer,
                                 bool hasClientId,
                                 bool withOutput,
                                 const QByteArray &clientid,
                                 quint32 messagenr,
                                 const QString &input,
                                 quint8 scpiType) :
    m_pPeer(peer),
    m_bhasClientId(hasClientId),
    m_bwithOutput(withOutput),
    m_clientId(clientid),
    m_nmessageNr(messagenr),
    m_sInput(input),
    m_nSCPIType(scpiType)
{
    qRegisterMetaType<ProtonetCommandPtr>();
    m_instanceCount++;
}

ProtonetCommand::ProtonetCommand(const ProtonetCommand &other) :
    m_pPeer(other.m_pPeer),
    m_bhasClientId(other.m_bhasClientId),
    m_bwithOutput(other.m_bwithOutput),
    m_clientId(other.m_clientId),
    m_nmessageNr(other.m_nmessageNr),
    m_sInput(other.m_sInput),
    m_nSCPIType(other.m_nSCPIType)
{
    m_instanceCount++;
}

ProtonetCommand::~ProtonetCommand()
{
    m_instanceCount--;
}

int ProtonetCommand::getInstanceCount()
{
    return m_instanceCount;
}
