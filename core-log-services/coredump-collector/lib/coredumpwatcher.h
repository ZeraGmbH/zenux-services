#ifndef COREDUMPWATCHER_H
#define COREDUMPWATCHER_H

#include <QObject>
#include <QList>

class CoreDumpWatcher : public QObject
{
    Q_OBJECT
public:
    explicit CoreDumpWatcher(QString coredumpDir, QList<int> userIdsToWatch);
signals:
    void sigCoredumpCreated();
};

#endif // COREDUMPWATCHER_H
