#include "sourcecontrolinterface.h"
#include "jsonstructapi.h"
#include "zera-jsonfileloader.h"
#include <zera-json-params-state.h>
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
    Q_UNUSED(leadingNodes)
    if (!m_sourceCapabilityFileName.isEmpty()) {
        QJsonObject capabilities = expandJsonCapabilities(cJsonFileLoader::loadJsonFile(m_sourceCapabilityFileName));
        if (!capabilities.isEmpty()) {
            QJsonDocument docCapabilities(capabilities);
            m_sourceCapabilities = docCapabilities.toJson();
            addDelegate("UISRC", "CAPABILITIES", SCPI::isQuery, m_scpiInterface, sourceCommands::cmdCapabilites);

            JsonStructApi structApi(capabilities);
            m_jsonSourceStateApi.setDeviceInfo(structApi.getDeviceName());
            QJsonDocument docSourceState(m_jsonSourceStateApi.getJsonStatus());
            m_sourceState = docSourceState.toJson();
            addDelegate("UISRC", "STATE", SCPI::isQuery, m_scpiInterface, sourceCommands::cmdState, &m_sourceState);

            ZeraJsonParamsState paramState(capabilities);
            QJsonDocument docDefaultState(paramState.getDefaultJsonState());
            m_sourceLoadState = docDefaultState.toJson();
            addDelegate("UISRC", "LOAD", SCPI::isQuery|SCPI::isCmdwP, m_scpiInterface, sourceCommands::cmdLoadState, &m_sourceLoadState);
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
            protoCmd->m_sOutput = m_sourceLoadState.getString();
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
