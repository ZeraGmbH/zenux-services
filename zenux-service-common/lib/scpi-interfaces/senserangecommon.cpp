#include "senserangecommon.h"
#include "zscpi_response_definitions.h"

const char* SenseRangeCommon::rangeNameCurrentNull = "0A";
const char* SenseRangeCommon::rangeAliasNull = "--";
static constexpr int ZenuxAdcMaxValue = (1<<23)-1; // Common value for COM/MT for the past decade

enum Commands
{
    cmdType,
    cmdAvail,
    cmdUpperRangeValue,
    cmdRejection,
    cmdOVRejection,
    cmdADCRejection,
    cmdCtrlSelection // attow just for test...
};

SenseRangeCommon::SenseRangeCommon(const std::shared_ptr<cSCPI> &scpiInterface,
                                   bool available,
                                   const SenseRangeCommonConfig &config,
                                   AdjRangeScpi *justdata) :
    ScpiConnection(scpiInterface),
    m_available(available),
    m_config(config),
    m_justdata(justdata)
{
}

SenseRangeCommon::~SenseRangeCommon()
{
    delete m_justdata;
}

void SenseRangeCommon::initSCPIConnection(const QString &leadingNodes)
{
    const QString adjLeadNodes = appendTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_config.m_name), "AVAIL", SCPI::isQuery, m_scpiInterface, cmdAvail);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_config.m_name), "URVALUE", SCPI::isQuery, m_scpiInterface, cmdUpperRangeValue);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_config.m_name), "REJECTION", SCPI::isQuery, m_scpiInterface, cmdRejection);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_config.m_name), "OVREJECTION", SCPI::isQuery, m_scpiInterface, cmdOVRejection);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_config.m_name), "ADCREJECTION", SCPI::isQuery, m_scpiInterface, cmdADCRejection);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_config.m_name), "TYPE", SCPI::isQuery, m_scpiInterface, cmdType);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_config.m_name), "CTRLSELECTION", SCPI::isQuery, m_scpiInterface, cmdCtrlSelection);

    connect(m_justdata, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_justdata->initSCPIConnection(QString("%1%2").arg(adjLeadNodes, getRangeName()));
}

QString SenseRangeCommon::getRangeName()
{
    return m_config.m_name;
}

double SenseRangeCommon::getUpperRangevalue() const
{
    return m_config.m_nominalValue;
}

quint8 SenseRangeCommon::getSelCode() const
{
    return m_config.m_nSelCode;
}

bool SenseRangeCommon::getAvail() const
{
    return m_available;
}

quint16 SenseRangeCommon::getMMask()
{
    return m_config.m_typeFlags;
}

void SenseRangeCommon::setMModeToUpdateAvailability(int mode)
{
    m_available = ((getMMask() & mode) > 0);
}

quint8 SenseRangeCommon::getAdjustmentStatus80Mask()
{
    return m_justdata->getAdjustmentStatus80Mask();
}

AdjRangeScpi *SenseRangeCommon::getJustData()
{
    return m_justdata;
}

bool SenseRangeCommon::initJustData()
{
    return m_justdata->initJustData();
}

bool SenseRangeCommon::computeJustData()
{
    return m_justdata->computeJustData();
}

void SenseRangeCommon::executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd)
{
    switch (cmdCode)
    {
    case cmdAvail:
        protoCmd->m_sOutput = scpiRangeAvail(protoCmd->m_sInput);
        break;
    case cmdUpperRangeValue:
        protoCmd->m_sOutput = scpiRangeUpperRangeValue(protoCmd->m_sInput);
        break;
    case cmdRejection:
        protoCmd->m_sOutput = scpiRangeRejection(protoCmd->m_sInput);
        break;
    case cmdOVRejection:
        protoCmd->m_sOutput = scpiRangeOVRejection(protoCmd->m_sInput);
        break;
    case cmdADCRejection:
        protoCmd->m_sOutput = scpiRangeADCRejection(protoCmd->m_sInput);
        break;
    case cmdType:
        protoCmd->m_sOutput = scpiRangeTypeFlags(protoCmd->m_sInput);
        break;
    case cmdCtrlSelection:
        protoCmd->m_sOutput = scpiControllerSelection(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString SenseRangeCommon::scpiRangeAvail(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_available ? "1" : "0";
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::scpiRangeUpperRangeValue(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_config.m_nominalValue);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::scpiRangeRejection(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_config.m_fRejection, 0, 'g', m_config.m_rejectionScpiQueryDigits);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::scpiRangeOVRejection(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_config.m_fOVRejection, 0, 'g', m_config.m_rejectionScpiQueryDigits);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::scpiRangeADCRejection(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(ZenuxAdcMaxValue);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::scpiRangeTypeFlags(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_config.m_typeFlags);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::scpiControllerSelection(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_config.m_nSelCode);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}
