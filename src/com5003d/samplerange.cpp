#include <QString>
#include <scpi.h>

#include "samplerange.h"
#include <protonetcommand.h>

cSampleRange::cSampleRange(cSCPI *scpiinterface, QString name, quint16 srate, quint8 selcode) :
    cSCPIConnection(scpiinterface),
    m_sName(name),
    m_nSRate(srate),
    m_nSelCode(selcode)
{
}


void cSampleRange::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"SRATE", SCPI::isQuery, m_pSCPIInterface, SampleRangeSamples);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
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


void cSampleRange::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SampleRangeSamples:
        protoCmd->m_sOutput = m_ReadSRate(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString cSampleRange::m_ReadSRate(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        return QString("%1").arg(m_nSRate);
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}
