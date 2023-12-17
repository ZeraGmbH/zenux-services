#include "com5003senserange.h"
#include "permissionfunctions.h"
#include "zscpi_response_definitions.h"

Com5003SenseRange::Com5003SenseRange(cSCPI *scpiinterface,
                                     QString name,
                                     QString alias,
                                     bool avail,
                                     double rValue,
                                     double rejection,
                                     double ovrejection,
                                     double adcrejection,
                                     quint8 rselcode) :
    SenseRangeCommon(
        scpiinterface,
        name,
        alias,
        avail,
        rValue,
        rejection,
        ovrejection,
        adcrejection,
        rselcode)
{
    m_pJustdata = new Com5003JustRangeTripletOffsetGainPhase(m_pSCPIInterface);
}


Com5003SenseRange::~Com5003SenseRange()
{
    delete m_pJustdata;
}


void Com5003SenseRange::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"TYPE",SCPI::isQuery,m_pSCPIInterface, SenseRange::cmdType);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS",SCPI::isQuery,m_pSCPIInterface, SenseRange::cmdAlias);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"AVAIL",SCPI::isQuery,m_pSCPIInterface, SenseRange::cmdAvail);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"URVALUE",SCPI::isQuery,m_pSCPIInterface, SenseRange::cmdValue);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"REJECTION",SCPI::isQuery,m_pSCPIInterface, SenseRange::cmdRejection);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"OVREJECTION",SCPI::isQuery,m_pSCPIInterface, SenseRange::cmdOVRejection);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ADCREJECTION",SCPI::isQuery,m_pSCPIInterface, SenseRange::cmdADCRejection);

    connect(m_pJustdata, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pJustdata->initSCPIConnection(QString("%1%2").arg(leadingNodes).arg(m_sName));
}


quint8 Com5003SenseRange::getAdjustmentStatus()
{
    return m_pJustdata->getAdjustmentStatus();
}


QString &Com5003SenseRange::getName()
{
    return m_sName;
}


double Com5003SenseRange::getUrvalue()
{
    return m_fRValue;
}


quint8 Com5003SenseRange::getSelCode()
{
    return m_nSelCode;
}


Com5003JustRangeTripletOffsetGainPhase *Com5003SenseRange::getJustData()
{
    return m_pJustdata;
}


bool Com5003SenseRange::getAvail()
{
    return m_bAvail;
}


void Com5003SenseRange::setAvail(bool b)
{
    m_bAvail = b;
}


void Com5003SenseRange::initJustData()
{
    m_pJustdata->initJustData();
}


void Com5003SenseRange::computeJustData()
{
    m_pJustdata->computeJustData();
}


void Com5003SenseRange::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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
    case SenseRange::cmdADCRejection:
        protoCmd->m_sOutput = m_ReadRangeADCRejection(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString Com5003SenseRange::m_ReadRangeType(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        // fix phys for compatibility
        // virtual ranges were never more than unimplemented great ideas
        return QString("%1").arg(0);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];

}


QString Com5003SenseRange::m_ReadRangeAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sAlias;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SenseRange::m_ReadRangeAvail(QString &sInput)
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


QString Com5003SenseRange::m_ReadRangeValue(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fRValue);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SenseRange::m_ReadRangeRejection(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fRejection, 0, 'g', 6);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SenseRange::m_ReadRangeOVRejection(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fOVRejection, 0, 'g', 6);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SenseRange::m_ReadRangeADCRejection(QString& sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fADCRejection, 0, 'g', 6);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}
