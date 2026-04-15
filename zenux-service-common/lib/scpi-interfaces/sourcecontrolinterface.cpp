#include "sourcecontrolinterface.h"
#include "zera-jsonfileloader.h"
#include "zscpi_response_definitions.h"
#include <zera-json-params-state.h>
#include <zera-json-params-structure.h>
#include <QJsonObject>
#include <QJsonDocument>

SourceControlInterface::SourceControlInterface(std::shared_ptr<cSCPI> scpiInterface,
                                               SourceControlSettings *settings,
                                               AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiServerConnection(scpiInterface),
    m_sourceCapabilityFileName(settings->getSourceCapFile()),
    m_ctrlFactory(ctrlFactory)
{
    Q_INIT_RESOURCE(scpi_interfaces);
}

enum sourceCommands {
    cmdCapabilites,
};

void SourceControlInterface::initSCPIConnection()
{
    QJsonObject capabilities = expandJsonCapabilities(cJsonFileLoader::loadJsonFile(m_sourceCapabilityFileName));
    if (!capabilities.isEmpty()) {
        QJsonDocument docCapabilities(capabilities);
        m_sourceCapabilities = docCapabilities.toJson();
        addDelegate("UISRC", "CAPABILITIES", SCPI::isQuery, m_scpiInterface, sourceCommands::cmdCapabilites);
    }
}

void SourceControlInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    bool respondsLater = false;
    cSCPICommand cmd = protoCmd->m_sInput;
    switch (cmdCode)
    {
    case sourceCommands::cmdCapabilites:
        if (cmd.isQuery())
            protoCmd->m_sOutput = m_sourceCapabilities;
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
        break;
    }
    if (protoCmd->m_bwithOutput && !respondsLater)
        emit cmdExecutionDone(protoCmd);
}

QJsonObject SourceControlInterface::expandJsonCapabilities(const QJsonObject &capabilitiesRaw)
{
    ZeraJsonParamsStructure jsonParamsStructure;
    jsonParamsStructure.setJson(capabilitiesRaw);
    return jsonParamsStructure.getJson();
}
