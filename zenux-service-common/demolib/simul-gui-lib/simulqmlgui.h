#ifndef SIMULQMLGUI_H
#define SIMULQMLGUI_H

#include <QQmlApplicationEngine>
#include <QObject>

class SimulQmlGui : public QObject
{
    Q_OBJECT
public:
    explicit SimulQmlGui();
private:
    QQmlApplicationEngine m_engine;
};

#endif // SIMULQMLGUI_H
