#include "mtxxxs2sensechannel.h"
#include "senserangecommon.h"
#include "zscpi_response_definitions.h"

MtXXXs2SenseChannel::MtXXXs2SenseChannel(std::shared_ptr<cSCPI> scpiinterface,
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

QString MtXXXs2SenseChannel::getAlias()
{
    return m_sAlias1;
}

void MtXXXs2SenseChannel::setNotifierSenseChannelRange()
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

QString MtXXXs2SenseChannel::scpiReadWriteRange(ProtonetCommandPtr protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isQuery())
        return notifierSenseChannelRange.getString();

    else if (cmd.isCommand(1)) {
        QString rangeName = cmd.getParam(0);
        SenseRangeCommon* range = getRange(rangeName);
        if ( range && range->getAvail() )
            return setRangeCommon(range, protoCmd);
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}
