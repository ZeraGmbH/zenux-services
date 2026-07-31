#ifndef COREDUMPWATCHER_H
#define COREDUMPWATCHER_H

#include <QObject>
#include <QList>
#include <QFileSystemWatcher>

class CoreDumpWatcher : public QObject
{
    Q_OBJECT
public:
    explicit CoreDumpWatcher(const QString &coreDumpDir, const QString &outputDir, const QList<int> &userIdsToWatch);
    void startWatching();

signals:
    void sigCoredumpMoved();

private slots:
    void newCoreDumpFound(const QString &path);

private:
    bool fixPermissions(const QString &filePath);
    int extractUserId(const QString &fileName);

    QString m_coreDumpDir;
    QString m_outputDir;
    QList<int> m_userIdsToWatch;
    QFileSystemWatcher m_watcher;

};

#endif // COREDUMPWATCHER_H
