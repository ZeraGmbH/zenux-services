#include "mt310s2senseinterface.h"
#include "mt310s2channelrangefactory.h"
#include "mt310s2dglobal.h"
#include "zscpi_response_definitions.h"

Mt310s2SenseInterface::Mt310s2SenseInterface(std::shared_ptr<cSCPI> scpiInterface,
                                             I2cSettings* i2cSettings,
                                             cSenseSettings* senseSettings,
                                             SystemInfo *systemInfo,
                                             AbstractFactoryI2cCtrlPtr ctrlFactory) :
    SenseInterfaceCommon(scpiInterface,
                           i2cSettings,
                           systemInfo,
                           ctrlFactory,
                           QHash<QString, int>{{"AC", modeAC}, {"HF", modeHF}, {"ADJ", modeADJ}})
{
    MT310s2ChannelRangeFactory factory;
    m_channelList = factory.createChannelAndRanges(senseSettings,
                                                   m_adjData,
                                                   m_scpiInterface,
                                                   m_ctrlFactory);
    injectAdjToChannelRanges();
    setSenseMode("AC");
    setNotifierSenseChannelCat(); // only prepared for !!! since we don't have hot plug for measuring channels yet
}

QString Mt310s2SenseInterface::getPcbName()
{
    return LeiterkartenName;
}

QString Mt310s2SenseInterface::getXmlType()
{
    return "PCBAdjustmentData";
}

bool Mt310s2SenseInterface::isRangePartOfAdjXmlExport(SenseRangeCommon *range)
{
    return range->getMMask() & Direct;
}

const char *Mt310s2SenseInterface::getAdjExportedVersion()
{
    return ServerVersion;
}

QString Mt310s2SenseInterface::scpiReadSenseGroupCatalog(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return ";"; // this server has no grouping constraints
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

bool Mt310s2SenseInterface::setSenseMode(QString sMode)
{
    if (m_availSenseModesHash.contains(sMode)) {
        quint8 mode = m_availSenseModesHash[sMode];
        for(auto channel : qAsConst(m_channelList))
            channel->setMMode(mode);
        m_currSenseMode = sMode;
        setNotifierSenseMMode();
        return true;
    }
    return false;
}
