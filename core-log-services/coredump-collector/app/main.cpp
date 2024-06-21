#include "coredumpwatcher.h"
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication(argc, argv);

    CoreDumpWatcher watcher("/var/lib/systemd/coredump/", "/var/lib/zenux-coredumps", QList<int>() << 10000 << 15000);
    watcher.startWatching();

    return app->exec();
}
