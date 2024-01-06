#include "com5003senserange.h"

static constexpr int rejectionScpiQueryDigitsCom5003 = 6;
static constexpr int adcRejectionCom5003 = (1<<23) -1;

Com5003SenseRange::Com5003SenseRange(cSCPI *scpiinterface,
                                     QString name,
                                     bool avail,
                                     double rValue,
                                     double rejection,
                                     double ovrejection,
                                     quint8 rselcode) :
    SenseRangeCommon(
        scpiinterface,
        name,
        avail,
        rValue,
        rejection,
        ovrejection,
        adcRejectionCom5003,
        rselcode,
        0, // no flags used in COM5003 implementation yet
        rejectionScpiQueryDigitsCom5003)
{
    m_pJustdata = new RangeAdjustmentInterface(m_pSCPIInterface, AdjustScpiValueFormatterFactory::createCom5003AdjFormatter());
}

Com5003SenseRange::~Com5003SenseRange()
{
    delete m_pJustdata;
}

void Com5003SenseRange::initSCPIConnection(QString leadingNodes)
{
    SenseRangeCommon::initSCPIConnection(leadingNodes);
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    connect(m_pJustdata, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pJustdata->initSCPIConnection(QString("%1%2").arg(leadingNodes).arg(getName()));
}

quint8 Com5003SenseRange::getAdjustmentStatus()
{
    return m_pJustdata->getAdjustmentStatus();
}

RangeAdjustmentInterface *Com5003SenseRange::getJustData()
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

