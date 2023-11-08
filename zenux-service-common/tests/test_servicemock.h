#ifndef TEST_SERVICEMOCK_H
#define TEST_SERVICEMOCK_H

#include <QObject>
#include <memory>

class test_servicemock : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void runMockMt310s2d();
    void connectMt310s2d();
    void connectInvalidIpMt310s2d();
    void getFoutCatMt310s2d();
private:
    void feedEventLoop();
};

#endif // TEST_SERVICEMOCK_H
