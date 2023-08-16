#include <QFile>
#include "accumulatorinterface.h"
#include "timerfactoryqt.h"

constexpr quint8 ERROR = 0x80;
constexpr int ACCU_POLLING_PERIOD_MS = 1000;

AccumulatorInterface::AccumulatorInterface(cSCPI *scpiInterface, cATMELSysCtrl *atmelSysCntrl, AccumulatorSettings *settings) :
    ScpiConnection(scpiInterface),
    m_atmelSysCntrl(atmelSysCntrl)
{
    m_pollingTimer = TimerFactoryQt::createPeriodic(ACCU_POLLING_PERIOD_MS);
    connect(m_pollingTimer.get(), &TimerTemplateQt::sigExpired, this, &AccumulatorInterface::getAccumulatorSoc);
    connect(m_pollingTimer.get(), &TimerTemplateQt::sigExpired, this, &AccumulatorInterface::getAccumulatorStatus);
    connect(m_pollingTimer.get(), &TimerTemplateQt::sigExpired, this, &AccumulatorInterface::setCpuTemperatur);
    //sendCpuTemperatur(quint16 cpuTemp)

    if(settings->isAvailable())
        m_pollingTimer->start();
}

void AccumulatorInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:ACCUMULATOR").arg(leadingNodes),"STATUS",SCPI::isQuery, m_pSCPIInterface, accumulatorCommands::cmdStatus, &m_accumulatorStatus);
    addDelegate(QString("%1SYSTEM:ACCUMULATOR").arg(leadingNodes),"SOC",SCPI::isQuery, m_pSCPIInterface, accumulatorCommands::cmdSoc, &m_accumulatorSoc);
}

void AccumulatorInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case accumulatorCommands::cmdStatus:
        protoCmd->m_sOutput = m_accumulatorStatus.getString();
        break;
    case accumulatorCommands::cmdSoc:
        protoCmd->m_sOutput = m_accumulatorSoc.getString();
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString AccumulatorInterface::getAccumulatorStatus()
{
    quint8 status = 0;
    if(m_atmelSysCntrl->readAccumulatorStatus(status) == ZeraMControllerIo::atmelRM::cmddone){
        m_accumulatorStatus = QString::number(status);
    }
    else{
        m_accumulatorStatus = QString::number(ERROR);
    }
    return m_accumulatorStatus.getString();
}

QString AccumulatorInterface::getAccumulatorSoc()
{
    quint8 charge = 0;
    if(m_atmelSysCntrl->readAccumulatorSoc(charge) == ZeraMControllerIo::atmelRM::cmddone){
        m_accumulatorSoc = QString::number(charge);
    }
    else{
        m_accumulatorSoc = QString::number(ERROR);
    }
    return m_accumulatorSoc.getString();
}

QString AccumulatorInterface::setCpuTemperatur()        // sends cyclic (1000ms) on I2C
{
    quint16 temperature = 34567;
    bool readTemp = false;

    QString fileName = {"/sys/class/thermal/thermal_zone0/temp"};
    QFile temperatureFile(fileName);
    //if(QFile::exists(temperatureFile))  // is: does not work ???
    if(temperatureFile.open(QFile::ReadOnly | QFile::Text))
    {
        qInfo("can open temp file");
        QString cpuTempStr = temperatureFile.readAll();
        temperatureFile.close();
        readTemp = true;
        temperature = cpuTempStr.toInt(&readTemp);
        if(!readTemp)
            qWarning("Conversion failed");
        else
            qInfo("CPU-Temp: %s", qPrintable(cpuTempStr));
    }
    else
        qWarning("Error opening temp-file");

    readTemp = true;


    if (readTemp)
    {
        if(m_atmelSysCntrl->sendCpuTemperatur(temperature) == ZeraMControllerIo::atmelRM::cmddone){
            m_CpuTemperatur = QString::number(temperature);
        }
        else{
            m_CpuTemperatur = QString::number(ERROR);
        }
        return m_CpuTemperatur.getString();
    }
    return("ERROR");
}



