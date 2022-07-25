#include <scpi.h>
#include <scpicommand.h>

#include "senserange.h"
#include "mt310s2justdata.h"
#include <scpidelegate.h>
#include <protonetcommand.h>


cSenseRange::cSenseRange(cSCPI *scpiinterface, QString name, QString alias, bool avail, double rValue, double rejection, double ovrejection, double adcrejection, quint8 rselcode, quint16 mmask, cMT310S2JustData* justdata) :
    cSCPIConnection(scpiinterface),
    m_sName(name),
    m_sAlias(alias),
    m_bAvail(avail),
    m_fRValue(rValue),
    m_fRejection(rejection),
    m_fOVRejection(ovrejection),
    m_fADCRejection(adcrejection),
    m_nSelCode(rselcode),
    m_nMMask(mmask),
    m_pJustdata(justdata)
{
}


cSenseRange::~cSenseRange()
{
    delete m_pJustdata;
}


void cSenseRange::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"TYPE",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdType);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdAlias);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"AVAIL",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdAvail);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"URVALUE",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdValue);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"REJECTION",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdRejection);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"OVREJECTION",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdOVRejection);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ADCREJECTION",SCPI::isQuery, m_pSCPIInterface, SenseRange::cmdADWRejection);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

    connect(m_pJustdata, SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
    m_pJustdata->initSCPIConnection(QString("%1%2").arg(leadingNodes).arg(m_sName));
}


quint8 cSenseRange::getAdjustmentStatus()
{
    return m_pJustdata->getAdjustmentStatus();
}


QString &cSenseRange::getName()
{
    return m_sName;
}


double cSenseRange::getUrvalue()
{
    return m_fRValue;
}


quint8 cSenseRange::getSelCode()
{
    return m_nSelCode;
}


quint16 cSenseRange::getMMask()
{
    return m_nMMask;
}


cMT310S2JustData *cSenseRange::getJustData()
{
    return m_pJustdata;
}


bool cSenseRange::isAvail()
{
    return m_bAvail;
}


void cSenseRange::setMMode(int m)
{
    m_nMMode = m;
    m_bAvail = ((m_nMMask & m_nMMode) > 0);
}


void cSenseRange::initJustData()
{
    m_pJustdata->initJustData();
}


void cSenseRange::computeJustData()
{
    m_pJustdata->computeJustData();
}


void cSenseRange::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SenseRange::cmdType:
        protoCmd->m_sOutput = m_ReadRangeType(protoCmd->m_sInput);
        break;
    case SenseRange::cmdAlias:
        protoCmd->m_sOutput = m_ReadRangeAlias(protoCmd->m_sInput);
        break;
    case SenseRange::cmdAvail:
        protoCmd->m_sOutput = m_ReadRangeAvail(protoCmd->m_sInput);
        break;
    case SenseRange::cmdValue:
        protoCmd->m_sOutput = m_ReadRangeValue(protoCmd->m_sInput);
        break;
    case SenseRange::cmdRejection:
        protoCmd->m_sOutput = m_ReadRangeRejection(protoCmd->m_sInput);
        break;
    case SenseRange::cmdOVRejection:
        protoCmd->m_sOutput = m_ReadRangeOVRejection(protoCmd->m_sInput);
        break;
    case SenseRange::cmdADWRejection:
        protoCmd->m_sOutput = m_ReadRangeADWRejection(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString cSenseRange::m_ReadRangeType(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_nMMask); // we return mmode mask and sensortype here
    else
        return SCPI::scpiAnswer[SCPI::nak];

}


QString cSenseRange::m_ReadRangeAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sAlias;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseRange::m_ReadRangeAvail(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_bAvail)
            return "1";
        else
            return "0";
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseRange::m_ReadRangeValue(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fRValue);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseRange::m_ReadRangeRejection(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fRejection, 0, 'g', 8);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseRange::m_ReadRangeOVRejection(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fOVRejection, 0, 'g', 8);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseRange::m_ReadRangeADWRejection(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fADCRejection, 0, 'g', 8);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

