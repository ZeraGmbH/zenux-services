#ifndef TEST_REGRESSION_SCPI_SEC1000_H
#define TEST_REGRESSION_SCPI_SEC1000_H

#include "mocksec1000d.h"
#include "secinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_scpi_sec1000 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase_data();
    void init();
    void cleanup();

    void serverUp();
    void dumpScpi();
private:
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockSec1000d> m_server;
    Zera::ProxyClientPtr m_proxyClient;
    Zera::cSECInterfacePtr m_secIFace;
};


#endif // TEST_REGRESSION_SCPI_SEC1000_H
