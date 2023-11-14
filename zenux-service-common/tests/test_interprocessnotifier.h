#ifndef TEST_INTERPROCESSNOTIFIER_H
#define TEST_INTERPROCESSNOTIFIER_H

#include <QObject>

class test_interprocessnotifier : public QObject
{
    Q_OBJECT
private slots:
    void openOnce();
    void openTwice();
    void openCloseOpen();
    void openCloseTwiceOpen();
    void sendOneSignal();
    void sendTwoSignals();
};

#endif // TEST_INTERPROCESSNOTIFIER_H
