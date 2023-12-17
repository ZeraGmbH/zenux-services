#include "senserangecommon.h"
#include "zscpi_response_definitions.h"

SenseRangeCommon::SenseRangeCommon(cSCPI *scpiInterface,
                                   QString name,
                                   QString alias,
                                   bool avail,
                                   double rValue,
                                   double rejection,
                                   double ovrejection,
                                   double adcrejection,
                                   quint8 rselcode) :
    ScpiConnection(scpiInterface),
    m_sName(name),
    m_sAlias(alias),
    m_bAvail(avail),
    m_fRValue(rValue),
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
}

QString &SenseRangeCommon::getName()
{
    return m_sName;
}

double SenseRangeCommon::getUrvalue() const
{
    return m_fRValue;
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
