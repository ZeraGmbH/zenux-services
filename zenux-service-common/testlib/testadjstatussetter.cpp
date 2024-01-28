#include "testadjstatussetter.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include <QStringList>

bool TestAdjStatusSetter::setAdjStatusAllChannelRanges(bool adjusted)
{
    QString adjStatSendVal = adjusted ? "128" : 0;
    QString channels = ScpiSingleTransactionBlocked::query("SENSE:CHANNEL:CATALOG?");
    QStringList channelList = channels.split(";");
    bool allOk = channelList.count() >= 6;
    for(const auto &channel : channelList) {
        QString ranges = ScpiSingleTransactionBlocked::query(QString("SENSE:%1:RANGE:CATALOG?").arg(channel));
        QStringList rangeList = ranges.split(";");
        for(const auto &range : rangeList) {
            QString ret = ScpiSingleTransactionBlocked::cmd(QString("SENSE:%1:%2:CORR:GAIN:STAT").arg(channel, range), adjStatSendVal);
            if(ret != ZSCPI::scpiAnswer[ZSCPI::ack])
                allOk = false;
            ret = ScpiSingleTransactionBlocked::cmd(QString("SENSE:%1:%2:CORR:PHASE:STAT").arg(channel, range), adjStatSendVal);
            if(ret != ZSCPI::scpiAnswer[ZSCPI::ack])
                allOk = false;
            ret = ScpiSingleTransactionBlocked::cmd(QString("SENSE:%1:%2:CORR:OFFSET:STAT").arg(channel, range), adjStatSendVal);
            if(ret != ZSCPI::scpiAnswer[ZSCPI::ack])
                allOk = false;
        }
    }
    if(ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "") != ZSCPI::scpiAnswer[ZSCPI::ack])
        allOk = false;
    // TODO? why is re-read necessary???
    if(ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "") != ZSCPI::scpiAnswer[ZSCPI::ack])
        allOk = false;
    return allOk;
}
