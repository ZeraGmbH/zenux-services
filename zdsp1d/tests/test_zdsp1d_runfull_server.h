#ifndef TEST_ZDSP1D_RUNFULL_SERVER_H
#define TEST_ZDSP1D_RUNFULL_SERVER_H

#include <QObject>

class test_zdsp1d_runfull_server : public QObject
{
    Q_OBJECT
private slots:
    void startServer();
private:
    void feedEventLoop();
};

#endif // TEST_ZDSP1D_RUNFULL_SERVER_H
