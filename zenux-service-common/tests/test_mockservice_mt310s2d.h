#ifndef TEST_MOCKSERVICE_MT310S2D_H
#define TEST_MOCKSERVICE_MT310S2D_H

#include "mockmt310s2d.h"
#include "resmanrunfacade.h"
#include <QObject>
#include <memory>

class test_mockservice_mt310s2d : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void runMock();
    void connectServer();
    void getFoutCat();
private:
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
};

#endif // TEST_MOCKSERVICE_MT310S2D_H
