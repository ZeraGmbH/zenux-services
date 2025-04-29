#include "protonetcommand.h"

int cProtonetCommand::m_instanceCount = 0;

cProtonetCommand::cProtonetCommand(VeinTcp::TcpPeer *peer, bool hasClientId, bool withOutput, QByteArray clientid, quint32 messagenr, QString input, quint8 scpiType) :
    m_pPeer(peer),
    m_bhasClientId(hasClientId),
    m_bwithOutput(withOutput),
    m_clientId(clientid),
    m_nmessageNr(messagenr),
    m_sInput(input),
    m_nSCPIType(scpiType)
{
    m_instanceCount++;
}

cProtonetCommand::cProtonetCommand(const cProtonetCommand *protoCmd) :
    m_pPeer(protoCmd->m_pPeer),
    m_bhasClientId(protoCmd->m_bhasClientId),
    m_bwithOutput(protoCmd->m_bwithOutput),
    m_clientId(protoCmd->m_clientId),
    m_nmessageNr(protoCmd->m_nmessageNr),
    m_sInput(protoCmd->m_sInput),
    m_nSCPIType(protoCmd->m_nSCPIType)
{
    m_instanceCount++;
}

cProtonetCommand::~cProtonetCommand()
{
    m_instanceCount--;
}

int cProtonetCommand::getInstanceCount()
{
    return m_instanceCount;
}
