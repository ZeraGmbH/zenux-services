#include "mt310s2senserange.h"
#include "mt310s2justdata.h"
#include "zscpi_response_definitions.h"
#include <math.h>

static constexpr int rejectionScpiQueryDigitsMt310s2 = 8;
static constexpr int adcRejectionCom5003 = (1<<23) -1;

Mt310s2SenseRange::Mt310s2SenseRange(cSCPI *scpiinterface,
                                     QString name,
                                     QString alias,
                                     bool avail,
                                     double rValue,
                                     double rejection,
                                     double ovrejection,
                                     quint8 rselcode,
                                     quint16 mmask,
                                     Mt310s2JustRangeTripletOffsetGainPhase* justdata) :
    SenseRangeCommon(
        scpiinterface,
        name,
        alias,
        avail,
        rValue,
        rejection,
        ovrejection,
        adcRejectionCom5003,
        rselcode,
        rejectionScpiQueryDigitsMt310s2),
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
    SenseRangeCommon::initSCPIConnection(leadingNodes);

    // the following are different from Com5003 so not yet ready to move to common
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "TYPE", SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdType);

    connect(m_pJustdata, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pJustdata->initSCPIConnection(QString("%1%2").arg(leadingNodes).arg(m_sName));
}

quint8 Mt310s2SenseRange::getAdjustmentStatus()
{
    return m_pJustdata->getAdjustmentStatus();
}

quint16 Mt310s2SenseRange::getMMask()
{
    return m_nMMask;
}

Mt310s2JustRangeTripletOffsetGainPhase *Mt310s2SenseRange::getJustData()
{
    return m_pJustdata;
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
    if(!execScpi(cmdCode, protoCmd)) {
        // the following are different from Com5003 so not yet ready to move to common
        switch (cmdCode)
        {
        case SenseRange::cmdType:
            protoCmd->m_sOutput = m_ReadRangeType(protoCmd->m_sInput);
            break;
        }
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
