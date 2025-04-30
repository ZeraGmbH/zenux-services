#include "senserangecommon.h"
#include "zscpi_response_definitions.h"

const char* SenseRangeCommon::rangeNameCurrentNull = "0A";
const char* SenseRangeCommon::rangeAliasNull = "--";
static constexpr int ZenuxAdcMaxValue = (1<<23)-1; // Common value for COM/MT for the past decade

enum Commands
{
    cmdType,
    cmdAlias,
    cmdAvail,
    cmdUpperRangeValue,
    cmdRejection,
    cmdOVRejection,
    cmdADCRejection,
    cmdCtrlSelection // attow just for test...
};

SenseRangeCommon::SenseRangeCommon(std::shared_ptr<cSCPI> scpiInterface,
                                   const QString &name,
                                   bool avail,
                                   double upperRangeValue,
                                   double rejection,
                                   double ovrejection,
                                   quint8 rselcode,
                                   quint32 typeFlags,
                                   AdjRangeScpi *justdata,
                                   int rejectionScpiQueryDigits) :
    ScpiConnection(scpiInterface),
    m_bAvail(avail),
    m_sName(name),
    m_sAlias(name != rangeNameCurrentNull ? name : rangeAliasNull),
    m_upperRangeValue(upperRangeValue),
    m_fRejection(rejection),
    m_fOVRejection(ovrejection),
    m_nSelCode(rselcode),
    m_typeFlags(typeFlags),
    m_rejectionScpiQueryDigits(rejectionScpiQueryDigits),
    m_justdata(justdata)
{
}

SenseRangeCommon::~SenseRangeCommon()
{
    delete m_justdata;
}

void SenseRangeCommon::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "ALIAS", SCPI::isQuery, m_scpiInterface, cmdAlias);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "AVAIL", SCPI::isQuery, m_scpiInterface, cmdAvail);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "URVALUE", SCPI::isQuery, m_scpiInterface, cmdUpperRangeValue);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "REJECTION", SCPI::isQuery, m_scpiInterface, cmdRejection);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "OVREJECTION", SCPI::isQuery, m_scpiInterface, cmdOVRejection);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "ADCREJECTION", SCPI::isQuery, m_scpiInterface, cmdADCRejection);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "TYPE", SCPI::isQuery, m_scpiInterface, cmdType);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "CTRLSELECTION", SCPI::isQuery, m_scpiInterface, cmdCtrlSelection);

    connect(m_justdata, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_justdata->initSCPIConnection(QString("%1%2").arg(leadingNodes, getRangeName()));
}

QString SenseRangeCommon::getRangeName()
{
    return m_sName;
}

double SenseRangeCommon::getUpperRangevalue() const
{
    return m_upperRangeValue;
}

quint8 SenseRangeCommon::getSelCode() const
{
    return m_nSelCode;
}

bool SenseRangeCommon::getAvail() const
{
    return m_bAvail;
}

quint16 SenseRangeCommon::getMMask()
{
    return m_typeFlags;
}

void SenseRangeCommon::setMModeToUpdateAvailability(int mode)
{
    m_bAvail = ((getMMask() & mode) > 0);
}

quint8 SenseRangeCommon::getAdjustmentStatus80Mask()
{
    return m_justdata->getAdjustmentStatus80Mask();
}

AdjRangeScpi *SenseRangeCommon::getJustData()
{
    return m_justdata;
}

void SenseRangeCommon::initJustData()
{
    m_justdata->initJustData();
}

void SenseRangeCommon::computeJustData()
{
    m_justdata->computeJustData();
}

void SenseRangeCommon::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode)
    {
    case cmdAlias:
        protoCmd->m_sOutput = scpiRangeAlias(protoCmd->m_sInput);
        break;
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

QString SenseRangeCommon::scpiRangeAlias(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_sAlias;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::scpiRangeAvail(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_bAvail ? "1" : "0";
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::scpiRangeUpperRangeValue(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_upperRangeValue);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::scpiRangeRejection(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_fRejection, 0, 'g', m_rejectionScpiQueryDigits);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::scpiRangeOVRejection(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_fOVRejection, 0, 'g', m_rejectionScpiQueryDigits);
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
        return QString("%1").arg(m_typeFlags);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseRangeCommon::scpiControllerSelection(const QString &scpi) const
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_nSelCode);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}
