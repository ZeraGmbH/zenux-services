#include "com5003sensechannel.h"
#include "senseinterfacecommon.h"
#include "senserangecommon.h"
#include "zscpi_response_definitions.h"

Com5003SenseChannel::Com5003SenseChannel(std::shared_ptr<cSCPI> scpiinterface,
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

QString Com5003SenseChannel::getAlias()
{
    if (m_nMMode == modeAC)
        return m_sAlias1;
    else
        return m_sAlias2;
}

void Com5003SenseChannel::setNotifierSenseChannelRange()
{
    quint8 mode;
    if (m_ctrlFactory->getMModeController()->readMeasMode(mode) == ZeraMControllerIo::cmddone ) {
        if (mode == modeAC) {
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
        else {
            if (mode == 1)
                notifierSenseChannelRange = "R0V";
            else
                notifierSenseChannelRange = "R10V";
        }
    }
    else
        notifierSenseChannelRange = m_RangeList.at(0)->getRangeName();
}

QString Com5003SenseChannel::scpiReadWriteRange(ProtonetCommandPtr protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isQuery())
        return notifierSenseChannelRange.getString();

    else if (cmd.isCommand(1)) {
        QString rangeName = cmd.getParam(0);
        SenseRangeCommon* range = getRange(rangeName);
        if ( range && range->getAvail() ) {
            if (m_nMMode == modeAC)
                return setRangeCommon(range, protoCmd);
            else {
                if (range->getRangeName() == "R0V") {
                    notifierSenseChannelRange = "R0V";
                    m_ctrlFactory->getMModeController()->setMeasMode(1);
                }
                else {
                    notifierSenseChannelRange = "R10V";
                    m_ctrlFactory->getMModeController()->setMeasMode(2);
                }
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}
