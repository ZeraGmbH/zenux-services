#include "sensechannelcommon.h"

SenseChannelCommon::SenseChannelCommon(cSCPI *scpiinterface,
                                       QString description,
                                       QString unit,
                                       SenseSystem::cChannelSettings *cSettings,
                                       quint8 nr,
                                       AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiConnection(scpiinterface),
    m_ctrlFactory(ctrlFactory),
    m_sDescription(description),
    m_sUnit(unit)
{
    m_sName = QString("m%1").arg(nr);
    m_sAlias1 = cSettings->m_sAlias1;
    m_sAlias2 = cSettings->m_sAlias2;
    m_nCtrlChannel = cSettings->m_nCtrlChannel;
    m_nDspChannel = cSettings->m_nDspChannel;
    m_nOverloadBit = cSettings->m_nOverloadBit;
    m_bAvail = cSettings->avail;
    m_nMMode = 0; // SenseChannel::modeAC the default
}

SenseChannelCommon::~SenseChannelCommon()
{
    for(auto range : qAsConst(m_RangeList)) {
        delete range;
    }
}
