#ifndef TEST_MOCKSERVICE_MT310S2D_FULL_H
#define TEST_MOCKSERVICE_MT310S2D_FULL_H

#include "mockmt310s2dfull.h"
#include "resmanrunfacade.h"
#include <QObject>
#include <memory>

class test_mockservice_mt310s2d_full : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void connectServer();
private:
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2dFull> m_mt310s2d;
};

#endif // TEST_MOCKSERVICE_MT310S2D_FULL_H
