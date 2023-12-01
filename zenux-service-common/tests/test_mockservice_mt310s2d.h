#ifndef TEST_MOCKSERVICE_MT310S2D_H
#define TEST_MOCKSERVICE_MT310S2D_H

#include <QObject>

class test_mockservice_mt310s2d : public QObject
{
    Q_OBJECT
private slots:
    void runMock();
    void connectServer();
    void connectInvalidIp();
    void getFoutCat();
private:
    void feedEventLoop();
};

#endif // TEST_MOCKSERVICE_MT310S2D_H
