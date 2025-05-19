#include "sourcecontrolinterface.h"
#include <QFile>

SourceControlInterface::SourceControlInterface(std::shared_ptr<cSCPI> scpiInterface,
                                               SourceControlSettings *settings,
                                               AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiConnection(scpiInterface),
    m_sourceCapabilityFile(settings->getSourceCapFile()),
    m_ctrlFactory(ctrlFactory)
{
    Q_INIT_RESOURCE(scpi_interfaces);
}

enum sourceCommands {
    cmdCapabilites,
};

void SourceControlInterface::initSCPIConnection(QString leadingNodes)
{
    if (!m_sourceCapabilityFile.isEmpty()) {
        QFile file(m_sourceCapabilityFile);
        if (file.open(QFile::ReadOnly)) {
            m_sourceCapabilities = file.readAll();
            if (!m_sourceCapabilities.isEmpty()) {
                ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
                addDelegate(QString("%1SOURCE").arg(leadingNodes),"CAPABILITIES",SCPI::isQuery, m_scpiInterface, sourceCommands::cmdCapabilites);
            }
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
