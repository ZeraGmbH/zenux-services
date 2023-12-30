#include "mt310s2senserange.h"
#include "mt310s2justdata.h"
#include <math.h>

static constexpr int rejectionScpiQueryDigitsMt310s2 = 8;
static constexpr int adcRejectionCom5003 = (1<<23) -1;

Mt310s2SenseRange::Mt310s2SenseRange(cSCPI *scpiinterface,
                                     QString name,
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
        avail,
        rValue,
        rejection,
        ovrejection,
        adcRejectionCom5003,
        rselcode,
        mmask,
        rejectionScpiQueryDigitsMt310s2),
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

    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    connect(m_pJustdata, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pJustdata->initSCPIConnection(QString("%1%2").arg(leadingNodes).arg(getName()));
}

quint8 Mt310s2SenseRange::getAdjustmentStatus()
{
    return m_pJustdata->getAdjustmentStatus();
}

Mt310s2JustRangeTripletOffsetGainPhase *Mt310s2SenseRange::getJustData()
{
    return m_pJustdata;
}

void Mt310s2SenseRange::setMMode(int mode)
{
    m_bAvail = ((getMMask() & mode) > 0);
}

void Mt310s2SenseRange::initJustData()
{
    m_pJustdata->initJustData();
}

void Mt310s2SenseRange::computeJustData()
{
    m_pJustdata->computeJustData();
}
