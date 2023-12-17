#include "senserangecommon.h"

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
