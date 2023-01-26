#include <scpi.h>
#include "emobinterface.h"

EmobInterface::EmobInterface(cI2CSettings *i2cSettings, cSCPI *pSCPIInterface) :
    ScpiConnection(pSCPIInterface),
    m_i2cSettings(i2cSettings),
    m_notifierChannelList(";")
{
}

void EmobInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:EMOB:CHANNEL").arg(leadingNodes),"CATALOG",SCPI::isQuery, m_pSCPIInterface, cmdChannelCat);
}

void EmobInterface::actualizeClampStatus(quint16 devConnectedMask)
{

}

void EmobInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case cmdChannelCat:
        protoCmd->m_sOutput = readChannelCatalog(protoCmd->m_sInput);
        break;
    /*case cmdVersionCTRL:
        protoCmd->m_sOutput = m_ReadCTRLVersion(protoCmd->m_sInput);
        break;*/
    }
    if (protoCmd->m_bwithOutput) {
        emit cmdExecutionDone(protoCmd);
    }
}

QString EmobInterface::readChannelCatalog(QString scpiCmd)
{
    cSCPICommand cmd = scpiCmd;
    if (cmd.isQuery()) {
        emit strNotifier(&m_notifierChannelList); // enable async notification on change
        return m_notifierChannelList.getString();
    }
    else {
        return SCPI::scpiAnswer[SCPI::nak];
    }
}
