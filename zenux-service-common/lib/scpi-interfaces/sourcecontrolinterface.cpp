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
};

void SourceControlInterface::initSCPIConnection(QString leadingNodes)
{
    if (!m_sourceCapabilityFileName.isEmpty()) {
        QJsonObject capabilities = expandJsonCapabilities(cJsonFileLoader::loadJsonFile(m_sourceCapabilityFileName));
        if (!capabilities.isEmpty()) {
            QJsonDocument doc(capabilities);
            m_sourceCapabilities = doc.toJson();

            ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
            addDelegate(QString("%1SOURCE").arg(leadingNodes),"CAPABILITIES",SCPI::isQuery, m_scpiInterface, sourceCommands::cmdCapabilites);
        }
    }
}

void SourceControlInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode)
    {
    case sourceCommands::cmdCapabilites: {
        protoCmd->m_sOutput = m_sourceCapabilities;
        break;
    }
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
