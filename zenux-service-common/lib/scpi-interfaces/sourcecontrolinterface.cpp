#include "sourcecontrolinterface.h"
#include "zera-jsonfileloader.h"
#include <zera-json-params-structure.h>
#include <QJsonObject>
#include <QJsonDocument>

SourceControlInterface::SourceControlInterface(std::shared_ptr<cSCPI> scpiInterface,
                                               SourceControlSettings *settings,
                                               AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiConnection(scpiInterface),
    m_sourceCapabilityFileName(settings->getSourceCapFile()),
    m_ctrlFactory(ctrlFactory)
{
    Q_INIT_RESOURCE(scpi_interfaces);
}

enum sourceCommands {
    cmdCapabilites,
    cmdState,
    cmdLoadState,
};

void SourceControlInterface::initSCPIConnection(QString leadingNodes)
{
    if (!m_sourceCapabilityFileName.isEmpty()) {
        QJsonObject capabilities = expandJsonCapabilities(cJsonFileLoader::loadJsonFile(m_sourceCapabilityFileName));
        if (!capabilities.isEmpty()) {
            QJsonDocument doc(capabilities);
            m_sourceCapabilities = doc.toJson();

            ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
            addDelegate(QString("%1UISRC").arg(leadingNodes),"CAPABILITIES",SCPI::isQuery, m_scpiInterface, sourceCommands::cmdCapabilites);
            addDelegate(QString("%1UISRC").arg(leadingNodes),"STATE",SCPI::isQuery, m_scpiInterface, sourceCommands::cmdState, &m_sourceState);
            addDelegate(QString("%1UISRC").arg(leadingNodes),"LOAD",SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, sourceCommands::cmdState, &m_sourceLoadState);
        }
    }
}

void SourceControlInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    switch (cmdCode)
    {
    case sourceCommands::cmdCapabilites:
        protoCmd->m_sOutput = m_sourceCapabilities;
        break;
    case sourceCommands::cmdState:
        protoCmd->m_sOutput = m_sourceState.getString();
        break;
    case sourceCommands::cmdLoadState:
        if (cmd.isQuery())
            protoCmd->m_sOutput = m_sourceState.getString();
        else {
            // TODO
        }
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QJsonObject SourceControlInterface::expandJsonCapabilities(const QJsonObject &capabilitiesRaw)
{
    ZeraJsonParamsStructure jsonParamsStructure;
    jsonParamsStructure.setJson(capabilitiesRaw);
    return jsonParamsStructure.getJson();
}
