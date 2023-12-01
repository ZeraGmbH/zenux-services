#ifndef TEST_MOCKSERVICE_COM5003D_H
#define TEST_MOCKSERVICE_COM5003D_H

#include <QObject>

class test_mockservice_com5003d : public QObject
{
    Q_OBJECT
private slots:
    void getFoutCat();
private:
    void feedEventLoop();
};

#endif // TEST_MOCKSERVICE_COM5003D_H
