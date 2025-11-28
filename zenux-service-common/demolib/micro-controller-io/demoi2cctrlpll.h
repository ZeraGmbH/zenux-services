#ifndef DEMOI2CCTRLPLL_H
#define DEMOI2CCTRLPLL_H

#include "abstractalli2ccontrollers.h"
#include "samplingsettings.h"

class DemoI2cCtrlPll : public AbstractI2cCtrlPll
{
public:
    DemoI2cCtrlPll(SamplingSettingsPtr samplingSettings, quint8& pllChannel);
    ZeraMControllerIo::atmelRM setPLLChannel(quint8 chn) override;
    ZeraMControllerIo::atmelRM readPLLChannel(quint8& chn) override;
private:
    void setSimulPllDisplay(quint8 chn);
    SamplingSettingsPtr m_samplingSettings;
    quint8& m_pllChannel;
};

#endif // DEMOI2CCTRLPLL_H
