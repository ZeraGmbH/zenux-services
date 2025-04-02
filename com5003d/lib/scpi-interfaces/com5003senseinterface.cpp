#include "com5003senseinterface.h"
#include "com5003channelrangefactory.h"
#include "com5003dglobal.h"
#include "zscpi_response_definitions.h"

Com5003SenseInterface::Com5003SenseInterface(std::shared_ptr<cSCPI> scpiInterface,
                                             I2cSettings *i2cSettings,
                                             cSenseSettings *senseSettings,
                                             SystemInfo *systemInfo,
                                             AbstractFactoryI2cCtrlPtr ctrlFactory) :
    SenseInterfaceCommon(scpiInterface,
                         i2cSettings,
                         systemInfo,
                         senseSettings,
                         std::make_shared<COM5003ChannelRangeFactory>(),
                         ctrlFactory,
                         QHash<QString, int>{{"AC", modeAC}, {"REF", modeREF}})
{
    injectAdjToChannelRanges();
    setSenseMode("AC");
    setNotifierSenseChannelCat(); // only prepared for !!! since we don't have hot plug for measuring channels yet
}

bool Com5003SenseInterface::setSenseMode(QString modeStr)
{
    if (m_availSenseModesHash.contains(modeStr)) {
        m_currSenseMode = modeStr;
        quint8 mode = m_availSenseModesHash[modeStr];
        for(auto channel : qAsConst(m_channelList))
            channel->setMMode(mode);
        changeSense();
        setNotifierSenseMMode();
        return true;
    }
    return false;
}

QString Com5003SenseInterface::scpiReadSenseGroupCatalog(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        if (m_availSenseModesHash[m_currSenseMode] == modeAC)
            return ";";
        else
            return "m0,m1,m2;m3,m4,m5;";
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void Com5003SenseInterface::changeSense()
{
    if (m_availSenseModesHash[m_currSenseMode] == modeAC) {
        m_channelList.at(0)->setUnit("V");
        m_channelList.at(1)->setUnit("V");
        m_channelList.at(2)->setUnit("V");

        m_channelList.at(3)->setUnit("A");
        m_channelList.at(4)->setUnit("A");
        m_channelList.at(5)->setUnit("A");
        m_ctrlFactory->getMModeController()->setMeasMode(0);
    }
    else { // REF
        for (qint32 i = 0; i < m_channelList.count(); i++) { // for each channel
            m_channelList.at(i)->setUnit("V");
        }
        // correct reference mode (1/'R0V' / 2/'R10V') are set in
        // Com5003SenseChannel::scpiReadWriteRange since the modes are treated
        // like ranges (that's why range grouping is mandatory in REF session)
        m_ctrlFactory->getMModeController()->setMeasMode(1);
    }
}

const char *Com5003SenseInterface::getAdjExportedVersion()
{
    return ServerVersion;
}

QString Com5003SenseInterface::getPcbName()
{
    return LeiterkartenName;
}

QString Com5003SenseInterface::getXmlType()
{
    return QString("%1AdjustmentData").arg(getPcbName());
}

bool Com5003SenseInterface::isRangePartOfAdjXmlExport(SenseRangeCommon *range)
{
    Q_UNUSED(range)
    // Once COM supports clamps, we have to revisit
    return true;
}
