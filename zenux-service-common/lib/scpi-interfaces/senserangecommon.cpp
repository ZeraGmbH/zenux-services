#include "senserangecommon.h"
#include "zscpi_response_definitions.h"

const char* SenseRangeCommon::rangeNameCurrentNull = "0A";
const char* SenseRangeCommon::rangeAliasNull = "--";

SenseRangeCommon::SenseRangeCommon(std::shared_ptr<cSCPI> scpiInterface,
                                   QString name,
                                   bool avail,
                                   double upperRangeValue,
                                   double rejection,
                                   double ovrejection,
                                   double adcrejection,
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
    m_fADCRejection(adcrejection),
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
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "ALIAS", SCPI::isQuery, m_scpiInterface, SenseRange::cmdAlias);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "AVAIL", SCPI::isQuery, m_scpiInterface, SenseRange::cmdAvail);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "URVALUE", SCPI::isQuery, m_scpiInterface, SenseRange::cmdUpperRangeValue);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "REJECTION", SCPI::isQuery, m_scpiInterface, SenseRange::cmdRejection);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "OVREJECTION", SCPI::isQuery, m_scpiInterface, SenseRange::cmdOVRejection);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "ADCREJECTION", SCPI::isQuery, m_scpiInterface, SenseRange::cmdADCRejection);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "TYPE", SCPI::isQuery, m_scpiInterface, SenseRange::cmdType);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName), "CTRLSELECTION", SCPI::isQuery, m_scpiInterface, SenseRange::cmdCtrlSelection);

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

void SenseRangeCommon::setAvail(bool avail)
{
    m_bAvail = avail;
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

void SenseRangeCommon::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SenseRange::cmdAlias:
        protoCmd->m_sOutput = scpiRangeAlias(protoCmd->m_sInput);
        break;
    case SenseRange::cmdAvail:
        protoCmd->m_sOutput = scpiRangeAvail(protoCmd->m_sInput);
        break;
    case SenseRange::cmdUpperRangeValue:
        protoCmd->m_sOutput = scpiRangeUpperRangeValue(protoCmd->m_sInput);
        break;
    case SenseRange::cmdRejection:
        protoCmd->m_sOutput = scpiRangeRejection(protoCmd->m_sInput);
        break;
    case SenseRange::cmdOVRejection:
        protoCmd->m_sOutput = scpiRangeOVRejection(protoCmd->m_sInput);
        break;
    case SenseRange::cmdADCRejection:
        protoCmd->m_sOutput = scpiRangeADCRejection(protoCmd->m_sInput);
        break;
    case SenseRange::cmdType:
        protoCmd->m_sOutput = scpiRangeTypeFlags(protoCmd->m_sInput);
        break;
    case SenseRange::cmdCtrlSelection:
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
        return QString("%1").arg(m_fADCRejection, 0, 'g', m_rejectionScpiQueryDigits);
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
