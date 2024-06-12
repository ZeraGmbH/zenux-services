#ifndef PERIODICLOGGERWRAPPER_H
#define PERIODICLOGGERWRAPPER_H

#include "periodiclogger.h"
#include <QObject>
#include <memory>

class PeriodicLoggerWrapper : public QObject
{
    Q_OBJECT
public:
    explicit PeriodicLoggerWrapper(QObject *parent = nullptr);
    void startPeriodicLog(QString logFilenameFull, int periodSeconds);
    void stopPeriodicLog();
    bool isLogActive();
signals:

private:
    std::unique_ptr<PeriodicLogger> m_logGenerator;
};

#endif // PERIODICLOGGERWRAPPER_H
