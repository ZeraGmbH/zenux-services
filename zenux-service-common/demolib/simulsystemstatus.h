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
    Q_PROPERTY(bool schnubbelPlugged READ getSchnubbelPlugged WRITE setSchnubbelPlugged NOTIFY schnubbelPluggedChanged FINAL)
    bool getSchnubbelPlugged();
    void setSchnubbelPlugged(bool newSchnubbelPlugged);
    Q_SIGNAL void schnubbelPluggedChanged();

    Q_PROPERTY(bool accuSupported READ accuSupported NOTIFY accuSupportedChanged FINAL)
    bool accuSupported() const;
    Q_SIGNAL void accuSupportedChanged();

    Q_PROPERTY(quint8 accuStateOfCharge READ getAccuStateOfCharge WRITE setAccuStateOfCharge NOTIFY accuStateOfChargeChanged FINAL)
    quint8 getAccuStateOfCharge();
    void setAccuStateOfCharge(quint8 newAccuStateOfCharge);
    Q_SIGNAL void accuStateOfChargeChanged();

    Q_PROPERTY(bool accuPresent READ accuPresent WRITE setAccuPresent NOTIFY accuPresentChanged FINAL)
    bool accuPresent() const;
    void setAccuPresent(bool newAccuPresent);
    Q_SIGNAL void accuPresentChanged();

    Q_PROPERTY(bool accuIsCharging READ accuIsCharging WRITE setAccuIsCharging NOTIFY accuIsChargingChanged FINAL)
    bool accuIsCharging() const;
    void setAccuIsCharging(bool newAccuIsCharging);
    Q_SIGNAL void accuIsChargingChanged();

    Q_PROPERTY(bool accuLowWarning READ accuLowWarning WRITE setAccuLowWarning NOTIFY accuLowWarningChanged FINAL)
    bool accuLowWarning() const;
    void setAccuLowWarning(bool newAccuLowWarning);
    Q_SIGNAL void accuLowWarningChanged();

    Q_PROPERTY(bool accuLowShutdownAlert READ accuLowShutdownAlert WRITE setAccuLowShutdownAlert NOTIFY accuLowShutdownAlertChanged FINAL)
    bool accuLowShutdownAlert() const;
    void setAccuLowShutdownAlert(bool newAccuLowShutdownAlert);
    Q_SIGNAL void accuLowShutdownAlertChanged();

    Q_PROPERTY(bool accuMainSupplyPresent READ accuMainSupplyPresent WRITE setAccuMainSupplyPresent NOTIFY accuMainSupplyPresentChanged FINAL)
    bool accuMainSupplyPresent() const;
    void setAccuMainSupplyPresent(bool newAccuMainSupplyPresent);
    Q_SIGNAL void accuMainSupplyPresentChanged();

    Q_PROPERTY(QString pllMode READ pllMode WRITE setPllMode NOTIFY pllModeChanged FINAL)
    QString pllMode() const;
    void setPllMode(const QString &newPllMode);
    Q_SIGNAL void pllModeChanged();

    Q_PROPERTY(QList<int> channelRanges READ channelRanges NOTIFY channelRangesChanged FINAL)
    QList<int> channelRanges() const;
    Q_SIGNAL void channelRangesChanged();

    // C++ only
    quint8 getAccuStatusFromFlags();
    quint8 getRange(quint8 channel);
    void setRange(quint8 channel, quint8 range);

private:
    SimulSystemStatus() = default;
    void makeAccuSupported();
    bool resizeChannelRanges(quint8 idx);

    static SimulSystemStatus* m_instance;

    bool m_schnubbelPlugged = true;

    bool m_accuSupported = false;

    bool m_accuPresent = false;
    bool m_accuIsCharging  = false;
    bool m_accuLowWarning = false;
    bool m_accuLowShutdownAlert = false;
    bool m_accuMainSupplyPresent = false;

    quint8 m_accuStateOfCharge = 0;

    QString m_pllMode = "Undefined";

    QList<int> m_channelRanges;
};

typedef std::shared_ptr<SimulSystemStatus> SimulSystemStatusPtr;

#endif // SIMULSYSTEMSTATUS_H
