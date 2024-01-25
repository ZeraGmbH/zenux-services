#include "sensechannelcommon.h"
#include "zscpi_response_definitions.h"

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

QString SenseChannelCommon::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return getAlias();
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadType(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("0");
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadUnit(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sUnit;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadDspChannel(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_nDspChannel);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadChannelStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        quint16 status;
        if (m_ctrlFactory->getCriticalStatusController()->readCriticalStatus(status) == ZeraMControllerIo::cmddone ) {
            quint32 r = ((m_bAvail) ? 0 : 1 << 31);
            if (m_nOverloadBit >= 0) { // perhaps this channel has no overload bit
                if ( (status & (1 << m_nOverloadBit))  > 0) {
                    r |= 1;
                }
            }
            return QString("%1").arg(r);
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_StatusReset(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if (m_nOverloadBit >= 0)  {
            if (m_ctrlFactory->getCriticalStatusController()->resetCriticalStatus((quint16)(1 << m_nOverloadBit)) == ZeraMControllerIo::cmddone ) {
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else {
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
            }
        }
        else {
            return ZSCPI::scpiAnswer[ZSCPI::ack];
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadUrvalue(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        for(auto range : qAsConst(m_RangeList)) {
            if (range->getName() == notifierSenseChannelRange.getString()) {
                return QString("%1").arg(range->getUpperRangevalue());
            }
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadRangeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return notifierSenseChannelRangeCat.getString();
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SenseChannelCommon::setNotifierSenseChannelRangeCat()
{
    int i;
    QString s;
    for (i = 0; i < m_RangeList.count()-1; i++)
        s += (m_RangeList.at(i)->getName() + ";");
    s += m_RangeList.at(i)->getName();
    notifierSenseChannelRangeCat = s;
}
