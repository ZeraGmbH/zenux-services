#include "demoi2cctrlpll.h"
#include "simulsystemstatus.h"

DemoI2cCtrlPll::DemoI2cCtrlPll(SamplingSettings* samplingSettings, quint8 &pllChannel) :
    m_samplingSettings(samplingSettings),
    m_pllChannel(pllChannel)
{
}

void DemoI2cCtrlPll::setSimulPllDisplay(quint8 chn)
{
    // Again some over engineering: Code is preparerd for multiple PLL or whatever settings
    // There have been one channel-setting eversince this code exist...
    QList<SamplingSettings::ChannelSettings*>& channelSettings = m_samplingSettings->getChannelSettings();
    if(channelSettings.count() > 0) {
        QStringList pllModes = channelSettings[0]->m_pllChannels;
        if(chn < pllModes.count())
            SimulSystemStatus::getInstance()->setPllMode(pllModes[chn]);
    }
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlPll::setPLLChannel(quint8 chn)
{
    m_pllChannel = chn;
    setSimulPllDisplay(chn);
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlPll::readPLLChannel(quint8 &chn)
{
    chn = m_pllChannel;
    return ZeraMControllerIo::atmelRM::cmddone;
}
