#include "simulsystemstatus.h"
#include "accustatusflags.h"

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

quint8 SimulSystemStatus::getAccuStatusFromFlags()
{
    // At the time of writing accu support is at multiple places. E.g one is
    // that com5003d does not contain AccumulatorInterface.
    // So until the situation is more clear deduce: When C++ polling is not
    // done, acumulator is not supported.
    makeAccuSupported();
    quint8 accuFlagMask = 0;
    if(m_accuPresent)
        accuFlagMask |= (1<<bp_Battery_Present);
    if(m_accuIsCharging)
        accuFlagMask |= (1<<bp_Battery_is_Charging);
    if(m_accuLowWarning)
        accuFlagMask |= (1<<bp_Battery_Low_SoC_Warning);
    if(m_accuLowShutdownAlert)
        accuFlagMask |= (1<<bp_Battery_Low_SoC_Shutdown_Alert);
    if(m_accuMainSupplyPresent)
        accuFlagMask |= (1<<bp_Main_Supply_Present);
    return accuFlagMask;
}

bool SimulSystemStatus::accuSupported() const
{
    return m_accuSupported;
}

void SimulSystemStatus::makeAccuSupported()
{
    if (m_accuSupported)
        return;
    m_accuSupported = true;
    emit accuSupportedChanged();
}

QString SimulSystemStatus::pllMode() const
{
    return m_pllMode;
}

void SimulSystemStatus::setPllMode(const QString &newPllMode)
{
    if (m_pllMode == newPllMode)
        return;
    m_pllMode = newPllMode;
    emit pllModeChanged();
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

bool SimulSystemStatus::accuPresent() const
{
    return m_accuPresent;
}

void SimulSystemStatus::setAccuPresent(bool newAccuPresent)
{
    if (m_accuPresent == newAccuPresent)
        return;
    m_accuPresent = newAccuPresent;
    emit accuPresentChanged();
}

bool SimulSystemStatus::accuIsCharging() const
{
    return m_accuIsCharging;
}

void SimulSystemStatus::setAccuIsCharging(bool newAccuIsCharging)
{
    if (m_accuIsCharging == newAccuIsCharging)
        return;
    m_accuIsCharging = newAccuIsCharging;
    emit accuIsChargingChanged();
}

bool SimulSystemStatus::accuLowWarning() const
{
    return m_accuLowWarning;
}

void SimulSystemStatus::setAccuLowWarning(bool newAccuLowWarning)
{
    if (m_accuLowWarning == newAccuLowWarning)
        return;
    m_accuLowWarning = newAccuLowWarning;
    emit accuLowWarningChanged();
}

bool SimulSystemStatus::accuLowShutdownAlert() const
{
    return m_accuLowShutdownAlert;
}

void SimulSystemStatus::setAccuLowShutdownAlert(bool newAccuLowShutdownAlert)
{
    if (m_accuLowShutdownAlert == newAccuLowShutdownAlert)
        return;
    m_accuLowShutdownAlert = newAccuLowShutdownAlert;
    emit accuLowShutdownAlertChanged();
}

bool SimulSystemStatus::accuMainSupplyPresent() const
{
    return m_accuMainSupplyPresent;
}

void SimulSystemStatus::setAccuMainSupplyPresent(bool newAccuMainSupplyPresent)
{
    if (m_accuMainSupplyPresent == newAccuMainSupplyPresent)
        return;
    m_accuMainSupplyPresent = newAccuMainSupplyPresent;
    emit accuMainSupplyPresentChanged();
}
