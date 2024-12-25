#include "samplerange.h"
#include "zscpi_response_definitions.h"

enum SampleRangeCommands
{
    SampleRangeSamples
};

cSampleRange::cSampleRange(std::shared_ptr<cSCPI> scpiinterface,
                           QString name,
                           quint16 srate,
                           quint8 selcode) :
    ScpiConnection(scpiinterface),
    m_sName(name),
    m_nSRate(srate),
    m_nSelCode(selcode)
{
}

void cSampleRange::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"SRATE", SCPI::isQuery, m_scpiInterface, SampleRangeSamples);
}

QString &cSampleRange::getName()
{
    return m_sName;
}

quint8 cSampleRange::getSelCode()
{
    return m_nSelCode;
}

quint16 cSampleRange::getSRate()
{
    return m_nSRate;
}

void cSampleRange::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SampleRangeSamples:
        protoCmd->m_sOutput = readSRate(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString cSampleRange::readSRate(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_nSRate);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}
