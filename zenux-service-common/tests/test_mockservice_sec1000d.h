#ifndef TEST_MOCKSERVICE_SEC1000D_H
#define TEST_MOCKSERVICE_SEC1000D_H

#include <QObject>

class test_mockservice_sec1000d : public QObject
{
    Q_OBJECT
private slots:
    void getChannelCatSec1000d();
private:
    void feedEventLoop();
};

#endif // TEST_MOCKSERVICE_SEC1000D_H
