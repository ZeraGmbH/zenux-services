#include "simulqmlgui.h"
#include "simulsystemstatus.h"

SimulQmlGui::SimulQmlGui()
{
    Q_INIT_RESOURCE(simulgui);
    qmlRegisterSingletonType<SimulSystemStatus>("SimulSystemStatus", 1, 0, "Simul", SimulSystemStatus::getInstance);
    m_engine.load(QUrl(QStringLiteral("qrc:/simul-qml/Main.qml")));
}
