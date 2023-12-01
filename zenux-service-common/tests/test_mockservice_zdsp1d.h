#ifndef TEST_MOCKSERVICE_ZDSP1D_H
#define TEST_MOCKSERVICE_ZDSP1D_H

#include "resmanrunfacade.h"
#include "mockzdsp1d.h"
#include <QObject>
#include <memory>

class test_mockservice_zdsp1d : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();
    void getDspLcaVersion();
    void getServerVersion();
private:
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockZdsp1d> m_zsdp1d;
};

#endif // TEST_MOCKSERVICE_ZDSP1D_H
