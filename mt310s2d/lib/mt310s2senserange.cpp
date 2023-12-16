#include "mt310s2senserange.h"
#include "mt310s2justdata.h"
#include "protonetcommand.h"
#include "zscpi_response_definitions.h"
#include <scpi.h>
#include <scpicommand.h>


Mt310s2SenseRange::Mt310s2SenseRange(cSCPI *scpiinterface, QString name, QString alias, bool avail, double rValue, double rejection, double ovrejection, double adcrejection, quint8 rselcode, quint16 mmask, Mt310s2JustRangeTripletOffsetGainPhase* justdata) :
    ScpiConnection(scpiinterface),
    m_sName(name),
    m_sAlias(alias),
    m_bAvail(avail),
    m_fRValue(rValue),
    m_fRejection(rejection),
    m_fOVRejection(ovrejection),
    m_fADCRejection(adcrejection),
    m_nSelCode(rselcode),
    m_nMMask(mmask),
    m_pJustdata(justdata)
{
}


Mt310s2SenseRange::~Mt310s2SenseRange()
{
    delete m_pJustdata;
}


void Mt310s2SenseRange::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"TYPE",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdType);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdAlias);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"AVAIL",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdAvail);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"URVALUE",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdValue);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"REJECTION",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdRejection);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"OVREJECTION",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdOVRejection);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ADCREJECTION",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdADWRejection);

    connect(m_pJustdata, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pJustdata->initSCPIConnection(QString("%1%2").arg(leadingNodes).arg(m_sName));
}


quint8 Mt310s2SenseRange::getAdjustmentStatus()
{
    return m_pJustdata->getAdjustmentStatus();
}


QString &Mt310s2SenseRange::getName()
{
    return m_sName;
}


double Mt310s2SenseRange::getUrvalue()
{
    return m_fRValue;
}


quint8 Mt310s2SenseRange::getSelCode()
{
    return m_nSelCode;
}


quint16 Mt310s2SenseRange::getMMask()
{
    return m_nMMask;
}


Mt310s2JustRangeTripletOffsetGainPhase *Mt310s2SenseRange::getJustData()
{
    return m_pJustdata;
}


bool Mt310s2SenseRange::isAvail()
{
    return m_bAvail;
}


void Mt310s2SenseRange::setMMode(int m)
{
    m_nMMode = m;
    m_bAvail = ((m_nMMask & m_nMMode) > 0);
}


void Mt310s2SenseRange::initJustData()
{
    m_pJustdata->initJustData();
}


void Mt310s2SenseRange::computeJustData()
{
    m_pJustdata->computeJustData();
}


void Mt310s2SenseRange::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SenseRange::cmdType:
        protoCmd->m_sOutput = m_ReadRangeType(protoCmd->m_sInput);
        break;
    case SenseRange::cmdAlias:
        protoCmd->m_sOutput = m_ReadRangeAlias(protoCmd->m_sInput);
        break;
    case SenseRange::cmdAvail:
        protoCmd->m_sOutput = m_ReadRangeAvail(protoCmd->m_sInput);
        break;
    case SenseRange::cmdValue:
        protoCmd->m_sOutput = m_ReadRangeValue(protoCmd->m_sInput);
        break;
    case SenseRange::cmdRejection:
        protoCmd->m_sOutput = m_ReadRangeRejection(protoCmd->m_sInput);
        break;
    case SenseRange::cmdOVRejection:
        protoCmd->m_sOutput = m_ReadRangeOVRejection(protoCmd->m_sInput);
        break;
    case SenseRange::cmdADWRejection:
        protoCmd->m_sOutput = m_ReadRangeADWRejection(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString Mt310s2SenseRange::m_ReadRangeType(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_nMMask); // we return mmode mask and sensortype here
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];

}


QString Mt310s2SenseRange::m_ReadRangeAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sAlias;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SenseRange::m_ReadRangeAvail(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_bAvail)
            return "1";
        else
            return "0";
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SenseRange::m_ReadRangeValue(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fRValue);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SenseRange::m_ReadRangeRejection(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fRejection, 0, 'g', 8);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SenseRange::m_ReadRangeOVRejection(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fOVRejection, 0, 'g', 8);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SenseRange::m_ReadRangeADWRejection(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fADCRejection, 0, 'g', 8);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}
