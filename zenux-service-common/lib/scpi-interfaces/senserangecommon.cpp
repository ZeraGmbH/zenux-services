#include "senserangecommon.h"
#include "zscpi_response_definitions.h"

SenseRangeCommon::SenseRangeCommon(cSCPI *scpiInterface,
                                   QString name,
                                   QString alias,
                                   bool avail,
                                   double upperRangeValue,
                                   double rejection,
                                   double ovrejection,
                                   double adcrejection,
                                   quint8 rselcode) :
    ScpiConnection(scpiInterface),
    m_sName(name),
    m_sAlias(alias),
    m_bAvail(avail),
    m_upperRangeValue(upperRangeValue),
    m_fRejection(rejection),
    m_fOVRejection(ovrejection),
    m_fADCRejection(adcrejection),
    m_nSelCode(rselcode)
{
}

void SenseRangeCommon::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "ALIAS", SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdAlias);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "AVAIL", SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdAvail);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "URVALUE", SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdUpperRangeValue);
}

QString &SenseRangeCommon::getName()
{
    return m_sName;
}

double SenseRangeCommon::getUpperRangevalue() const
{
    return m_upperRangeValue;
}

quint8 SenseRangeCommon::getSelCode() const
{
    return m_nSelCode;
}

bool SenseRangeCommon::getAvail() const
{
    return m_bAvail;
}

void SenseRangeCommon::setAvail(bool avail)
{
    m_bAvail = avail;
}

QString SenseRangeCommon::handeScpiRangeAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sAlias;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::handeScpiRangeAvail(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_bAvail ? "1" : "0";
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::handeScpiRangeUpperRangeValue(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_upperRangeValue);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}
