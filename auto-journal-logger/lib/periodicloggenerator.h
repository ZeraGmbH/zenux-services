#ifndef PERIODICLOGGENERATOR_H
#define PERIODICLOGGENERATOR_H

#include "abstractlogcreator.h"
#include <timertemplateqt.h>
#include <QObject>

class PeriodicLogGenerator : public QObject
{
    Q_OBJECT
public:
    PeriodicLogGenerator(AbstractLogCreatorPtr logCreator);
    bool saveLogFileNow();
    void startPeriodicLog(int storeFreqMs);
    void stopPeriodicLog();

private slots:
    void onSaveTimer();
private:
    AbstractLogCreatorPtr m_logCreator;
    TimerTemplateQtPtr m_timer;
};

#endif // PERIODICLOGGENERATOR_H
