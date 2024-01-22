#include "simulsystemstatus.h"

SimulSystemStatus* SimulSystemStatus::m_instance = nullptr;

SimulSystemStatus *SimulSystemStatus::getInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    if(!m_instance)
        m_instance = new SimulSystemStatus;
    return m_instance;
}

bool SimulSystemStatus::getSchnubbelPlugged()
{
    return m_schnubbelPlugged;
}

void SimulSystemStatus::setSchnubbelPlugged(bool newSchnubbelPlugged)
{
    if (m_schnubbelPlugged == newSchnubbelPlugged)
        return;
    m_schnubbelPlugged = newSchnubbelPlugged;
    emit schnubbelPluggedChanged();
}

quint8 SimulSystemStatus::getAccuStatus()
{
    return m_accuStatus;
}

void SimulSystemStatus::setAccuStatus(quint8 newAccuStatus)
{
    if (m_accuStatus == newAccuStatus)
        return;
    m_accuStatus = newAccuStatus;
    emit accuStatusChanged();
}

quint8 SimulSystemStatus::getAccuStateOfCharge()
{
    return m_accuStateOfCharge;
}

void SimulSystemStatus::setAccuStateOfCharge(quint8 newAccuStateOfCharge)
{
    if (m_accuStateOfCharge == newAccuStateOfCharge)
        return;
    m_accuStateOfCharge = newAccuStateOfCharge;
    emit accuStateOfChargeChanged();
}
