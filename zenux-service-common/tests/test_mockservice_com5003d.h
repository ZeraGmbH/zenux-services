#ifndef TEST_MOCKSERVICE_COM5003D_H
#define TEST_MOCKSERVICE_COM5003D_H

#include "resmanrunfacade.h"
#include "mockcom5003d.h"
#include <QObject>
#include <memory>

class test_mockservice_com5003d : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void getFoutCat();
private:
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockCom5003d> m_com5003d;
};

#endif // TEST_MOCKSERVICE_COM5003D_H
