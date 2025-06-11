#include "sourcecontrolinterface.h"
#include "jsonstructapi.h"
#include "zera-jsonfileloader.h"
#include "zscpi_response_definitions.h"
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
    case sourceCommands::cmdState:
        if (cmd.isQuery())
            protoCmd->m_sOutput = m_sourceState.getString();
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
        break;
    case sourceCommands::cmdLoadState:
        if (cmd.isQuery())
            protoCmd->m_sOutput = m_sourceLoadState.getString();
        else {
            QJsonObject jsonCapabilies = QJsonDocument::fromJson(m_sourceCapabilities.toUtf8()).object();
            ZeraJsonParamsState paramState(jsonCapabilies);
            QString strParam = cmd.getParam();
            QJsonObject param = QJsonDocument::fromJson(strParam.toUtf8()).object();
            ZeraJsonParamsState::ErrList errs = paramState.validateJsonState(param);
            if (errs.isEmpty()) {
                // TODO: Do something
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
        }
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
