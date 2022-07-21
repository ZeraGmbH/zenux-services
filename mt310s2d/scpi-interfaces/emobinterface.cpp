#include <scpi.h>
#include "emobinterface.h"

EmobInterface::EmobInterface(cI2CSettings *i2cSettings, cSCPI *pSCPIInterface) :
    cSCPIConnection(pSCPIInterface),
    m_i2cSettings(i2cSettings),
    m_notifierChannelList(";")
{
}

void EmobInterface::initSCPIConnection(QString leadingNodes)
{
    if (leadingNodes != "") {
        leadingNodes += ":";
    }
    cSCPIDelegate* delegate = new cSCPIDelegate(QString("%1SYSTEM:EMOB:CHANNEL").arg(leadingNodes),"CATALOG",SCPI::isQuery, m_pSCPIInterface, cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
}

void EmobInterface::actualizeClampStatus(quint16 devConnectedMask)
{

}

void EmobInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
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
