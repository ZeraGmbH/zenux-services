#ifndef COREDUMPWATCHER_H
#define COREDUMPWATCHER_H

#include <QObject>
#include <QList>
#include <QFileSystemWatcher>

class CoreDumpWatcher : public QObject
{
    Q_OBJECT
public:
    explicit CoreDumpWatcher(QString coreDumpDir, QString outputDir, QList<int> userIdsToWatch);
    void startWatching();

signals:
    void sigCoredumpCreated();

private slots:
    void newCoreDumpFound(QString path);

private:
    bool fixPermissions(QString filePath);
    int extractUserId(QString fileName);

    QString m_coreDumpDir;
    QString m_outputDir;
    QList<int> m_userIdsToWatch;
    QFileSystemWatcher m_watcher;

};

#endif // COREDUMPWATCHER_H
