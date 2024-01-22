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

    bool getSchnubbelPlugged();
    void setSchnubbelPlugged(bool newSchnubbelPlugged);

    quint8 getAccuStatus();
    void setAccuStatus(quint8 newAccuStatus);

    quint8 getAccuStateOfCharge();
    void setAccuStateOfCharge(quint8 newAccuStateOfCharge);

signals:
    void schnubbelPluggedChanged();
    void accuStatusChanged();
    void accuStateOfChargeChanged();

private:
    SimulSystemStatus() = default;
    static SimulSystemStatus* m_instance;

    bool m_schnubbelPlugged = true;
    Q_PROPERTY(bool schnubbelPlugged READ getSchnubbelPlugged WRITE setSchnubbelPlugged NOTIFY schnubbelPluggedChanged FINAL)
    quint8 m_accuStatus = 0;
    Q_PROPERTY(quint8 accuStatus READ getAccuStatus WRITE setAccuStatus NOTIFY accuStatusChanged FINAL)
    quint8 m_accuStateOfCharge = 0;
    Q_PROPERTY(quint8 accuStateOfCharge READ getAccuStateOfCharge WRITE setAccuStateOfCharge NOTIFY accuStateOfChargeChanged FINAL)
};

typedef std::shared_ptr<SimulSystemStatus> SimulSystemStatusPtr;

#endif // SIMULSYSTEMSTATUS_H
