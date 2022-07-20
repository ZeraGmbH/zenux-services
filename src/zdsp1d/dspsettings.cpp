#include <QVariant>
#include <xmlconfigreader.h>

#include "dspsettings.h"


cDSPSettings::cDSPSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["zdsp1dconfig:connectivity:dsp:device:node"] = DSPSettings::setDSPDevNode;
    m_ConfigXMLMap["zdsp1dconfig:dspsettings:bootfile"] = DSPSettings::setDSPBootfile;
    m_ConfigXMLMap["zdsp1dconfig:dspsettings:boot"] = DSPSettings::setDSPBoot;
    m_ConfigXMLMap["zdsp1dconfig:dspsettings:samplingsystem:channels"] = DSPSettings::setDSPChannelNr;
    m_ConfigXMLMap["zdsp1dconfig:dspsettings:samplingsystem:signalperiod"] = DSPSettings::setDSPSignalPeriod;
    m_ConfigXMLMap["zdsp1dconfig:dspsettings:samplingsystem:measureperiod"] = DSPSettings::setDSPMeasPeriod;
}


QString& cDSPSettings::getDeviceNode()
{
    return m_sDeviceNode;
}


QString &cDSPSettings::getBootFile()
{
    return m_sBootFile;
}


quint8 cDSPSettings::getChannelNr()
{
    return m_nChannels;
}


bool cDSPSettings::isBoot()
{
    return (m_nBoot == 1);
}


quint16 cDSPSettings::getSamplesSignalPeriod()
{
    return m_nSamplesPerSignalPeriod;
}


quint16 cDSPSettings::getsamplesMeasurePeriod()
{
    return m_nSamplesPerMeasurePeriod;
}


void cDSPSettings::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        switch (m_ConfigXMLMap[key])
        {
        case DSPSettings::setDSPDevNode:
            m_sDeviceNode = m_pXMLReader->getValue(key);
            break;
        case DSPSettings::setDSPBootfile:
            m_sBootFile = m_pXMLReader->getValue(key);
            break;
        case DSPSettings::setDSPBoot:
            m_nBoot = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case DSPSettings::setDSPChannelNr:
            m_nChannels = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case DSPSettings::setDSPMeasPeriod:
            m_nSamplesPerMeasurePeriod = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case DSPSettings::setDSPSignalPeriod:
            m_nSamplesPerSignalPeriod = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        }
    }
}
