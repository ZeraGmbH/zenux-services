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
    quint8 mode, rSelCode;
    if (m_ctrlFactory->getMModeController()->readMeasMode(mode) == ZeraMControllerIo::cmddone ) {
        if (mode == modeAC) {
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

QString Com5003SenseChannel::scpiReadWriteRange(QString &sInput)
{
    cSCPICommand cmd = sInput;
    quint8 mode;
    if (m_ctrlFactory->getMModeController()->readMeasMode(mode) == ZeraMControllerIo::cmddone ) {
        if (cmd.isQuery())
            return notifierSenseChannelRange.getString();
        else if (cmd.isCommand(1)) {
            QString rng = cmd.getParam(0);
            int anz = m_RangeList.count();
            int i;
            for  (i = 0; i < anz; i++) {
                if (m_RangeList.at(i)->getRangeName() == rng)
                    break;
            }
            if ( (i < anz) && (m_RangeList.at(i)->getAvail()) ) {
                // we know this range and it's available
                if (m_nMMode == modeAC) {
                    if (m_ctrlFactory->getRangesController()->setRange(m_nCtrlChannel, m_RangeList.at(i)->getSelCode()) == ZeraMControllerIo::cmddone) {
                        notifierSenseChannelRange = rng;
                        return ZSCPI::scpiAnswer[ZSCPI::ack];
                    }
                    else
                        return ZSCPI::scpiAnswer[ZSCPI::errexec];
                }
                else {
                    if (m_RangeList.at(i)->getRangeName() == "R0V") {
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
    return ZSCPI::scpiAnswer[ZSCPI::errexec];
}
