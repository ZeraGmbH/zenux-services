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
    SenseRangeCommon::initSCPIConnection(leadingNodes);

    // the following are different from Mt310s2 so not yet ready to move to common
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "TYPE", SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdType);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "REJECTION", SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdRejection);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "OVREJECTION", SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdOVRejection);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "ADCREJECTION", SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdADCRejection);

    connect(m_pJustdata, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pJustdata->initSCPIConnection(QString("%1%2").arg(leadingNodes).arg(m_sName));
}

quint8 Com5003SenseRange::getAdjustmentStatus()
{
    return m_pJustdata->getAdjustmentStatus();
}

Com5003JustRangeTripletOffsetGainPhase *Com5003SenseRange::getJustData()
{
    return m_pJustdata;
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
    if(!execScpi(cmdCode, protoCmd)) {
        // the following are different from Mt310s2 so not yet ready to move to common
        switch (cmdCode)
        {
        case SenseRange::cmdType:
            protoCmd->m_sOutput = m_ReadRangeType(protoCmd->m_sInput);
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