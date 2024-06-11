#ifndef ONOFFFILEWATCHER_H
#define ONOFFFILEWATCHER_H

#include <QObject>
#include <QFileSystemWatcher>

class OnOffFileWatcher : public QObject
{
    Q_OBJECT
public:
    OnOffFileWatcher();
    bool isOn();
    static QString getOnOffFullPath();
    static QString getOnOffDir();
signals:
    void sigOnOffChangeDetected();

private slots:
    void onDirectoryChanged();
private:
    QFileSystemWatcher m_dirWatcher;
};

#endif // ONOFFFILEWATCHER_H
