#ifndef SIMULSYSTEMSTATUS_H
#define SIMULSYSTEMSTATUS_H

#include <QObject>
#include <memory>

// keep out Qml -> forward
class QQmlEngine;
class QJSEngine;

class SimulSystemStatus : public QObject
{
    Q_OBJECT
public:
    static SimulSystemStatus *getInstance(QQmlEngine *engine = nullptr, QJSEngine *scriptEngine = nullptr);

    // QML / C++
    bool getSchnubbelPlugged();
    void setSchnubbelPlugged(bool newSchnubbelPlugged);

    quint8 getAccuStateOfCharge();
    void setAccuStateOfCharge(quint8 newAccuStateOfCharge);

    bool accuPresent() const;
    void setAccuPresent(bool newAccuPresent);

    bool accuIsCharging() const;
    void setAccuIsCharging(bool newAccuIsCharging);

    bool accuLowWarning() const;
    void setAccuLowWarning(bool newAccuLowWarning);

    bool accuLowShutdownAlert() const;
    void setAccuLowShutdownAlert(bool newAccuLowShutdownAlert);

    bool accuMainSupplyPresent() const;
    void setAccuMainSupplyPresent(bool newAccuMainSupplyPresent);

    // C++ only
    quint8 getAccuStatusFromFlags();

signals:
    void schnubbelPluggedChanged();

    void accuStateOfChargeChanged();
    void accuPresentChanged();
    void accuIsChargingChanged();
    void accuLowWarningChanged();
    void accuLowShutdownAlertChanged();
    void accuMainSupplyPresentChanged();

private:
    SimulSystemStatus() = default;
    static SimulSystemStatus* m_instance;

    Q_PROPERTY(bool schnubbelPlugged READ getSchnubbelPlugged WRITE setSchnubbelPlugged NOTIFY schnubbelPluggedChanged FINAL)
    bool m_schnubbelPlugged = true;

    Q_PROPERTY(bool accuPresent READ accuPresent WRITE setAccuPresent NOTIFY accuPresentChanged FINAL)
    bool m_accuPresent = false;
    Q_PROPERTY(bool accuIsCharging READ accuIsCharging WRITE setAccuIsCharging NOTIFY accuIsChargingChanged FINAL)
    bool m_accuIsCharging  = false;
    Q_PROPERTY(bool accuLowWarning READ accuLowWarning WRITE setAccuLowWarning NOTIFY accuLowWarningChanged FINAL)
    bool m_accuLowWarning = false;
    Q_PROPERTY(bool accuLowShutdownAlert READ accuLowShutdownAlert WRITE setAccuLowShutdownAlert NOTIFY accuLowShutdownAlertChanged FINAL)
    bool m_accuLowShutdownAlert = false;
    Q_PROPERTY(bool accuMainSupplyPresent READ accuMainSupplyPresent WRITE setAccuMainSupplyPresent NOTIFY accuMainSupplyPresentChanged FINAL)
    bool m_accuMainSupplyPresent = false;

    Q_PROPERTY(quint8 accuStateOfCharge READ getAccuStateOfCharge WRITE setAccuStateOfCharge NOTIFY accuStateOfChargeChanged FINAL)
    quint8 m_accuStateOfCharge = 0;
};

typedef std::shared_ptr<SimulSystemStatus> SimulSystemStatusPtr;

#endif // SIMULSYSTEMSTATUS_H
