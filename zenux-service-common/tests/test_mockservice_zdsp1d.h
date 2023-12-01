#ifndef TEST_MOCKSERVICE_ZDSP1D_H
#define TEST_MOCKSERVICE_ZDSP1D_H

#include <QObject>

class test_mockservice_zdsp1d : public QObject
{
    Q_OBJECT
private slots:
    void getVersion();
private:
    void feedEventLoop();
};

#endif // TEST_MOCKSERVICE_ZDSP1D_H
