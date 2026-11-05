#include "mtxxxs2sensechannel.h"
#include "senserangecommon.h"
#include "zscpi_response_definitions.h"

MtXXXs2SenseChannel::MtXXXs2SenseChannel(const std::shared_ptr<cSCPI> &scpiInterface,
                                         const QString &unit,
                                         SenseSystem::cChannelSettings *cSettings,
                                         const AbstractFactoryI2cCtrlPtr &ctrlFactory) :
    SenseChannelCommon(scpiInterface,
                       unit,
                       cSettings,
                       ctrlFactory)
{
}

QString MtXXXs2SenseChannel::getAlias()
{
    return m_sAlias1;
}

void MtXXXs2SenseChannel::refreshNotifierForActualRange()
{
    quint8 rSelCode;
    if (m_ctrlFactory->getRangesController()->readRange(m_nCtrlChannel, rSelCode) == ZeraMControllerIo::cmddone ) {
        for(auto range : qAsConst(m_RangeList)) {
            if ( (range->getSelCode() == rSelCode) && (range->getAvail())) {
                notifierSenseChannelRange = range->getRangeName();
                return;
            }
        }
        qWarning("On channel %s range for selection code %i was not found!", qPrintable(m_sAlias1), rSelCode);
    }
    else
        qWarning("Reading range for notification failed for %s", qPrintable(m_sAlias1));
}

SenseChannelCommon::NotificationStatus MtXXXs2SenseChannel::scpiReadWriteRange(const ProtonetCommandPtr &protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isQuery()) {
        protoCmd->m_sOutput = notifierSenseChannelRange.getString();
        return SenseChannelCommon::NotificationNow;
    }

    else if (cmd.isCommand(1)) {
        QString rangeName = cmd.getParam(0);
        SenseRangeCommon* range = getRange(rangeName);
        if ( range && range->getAvail() )
            return setRangeCommon(range, protoCmd);
    }
    protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
    return SenseChannelCommon::NotificationNow;
}
