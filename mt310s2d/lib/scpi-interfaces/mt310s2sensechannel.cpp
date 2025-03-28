#include "mt310s2sensechannel.h"
#include "senserangecommon.h"
#include "zscpi_response_definitions.h"

Mt310s2SenseChannel::Mt310s2SenseChannel(std::shared_ptr<cSCPI> scpiinterface,
                                         QString unit,
                                         SenseSystem::cChannelSettings *cSettings,
                                         quint8 nr,
                                         AbstractFactoryI2cCtrlPtr ctrlFactory) :
    SenseChannelCommon(scpiinterface,
                       unit,
                       cSettings,
                       nr,
                       ctrlFactory)
{
}

QString Mt310s2SenseChannel::getAlias()
{
    return m_sAlias1;
}

void Mt310s2SenseChannel::setNotifierSenseChannelRange()
{
    quint8 rSelCode;
    if (m_ctrlFactory->getRangesController()->readRange(m_nCtrlChannel, rSelCode) == ZeraMControllerIo::cmddone ) {
        for(auto range : qAsConst(m_RangeList)) {
            if ( (range->getSelCode() == rSelCode) && (range->getAvail())) {
                notifierSenseChannelRange = range->getRangeName();
                break;
            }
        }
    }
}

QString Mt310s2SenseChannel::scpiReadWriteRange(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return notifierSenseChannelRange.getString();
    else if (cmd.isCommand(1)) {
        QString rng = cmd.getParam(0);
        int anz = m_RangeList.count();
        int i;
        for (i = 0; i < anz; i++) {
            if (m_RangeList.at(i)->getRangeName() == rng)
                break;
        }
        if ( (i < anz) && (m_RangeList.at(i)->getAvail()) ) {
            // we know this range and it's available
            if (m_ctrlFactory->getRangesController()->setRange(m_nCtrlChannel, m_RangeList.at(i)->getSelCode()) == ZeraMControllerIo::cmddone) {
                notifierSenseChannelRange = rng;
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}
