#ifndef TEST_REGRESSION_ADJ_IMPORT_PERMISSION_COM5003_H
#define TEST_REGRESSION_ADJ_IMPORT_PERMISSION_COM5003_H

#include "testserverforsenseinterfacecom5003.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_adj_regression_import_permission_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void scpiImportPermissionQueryFail();
    void scpiImportNoPermission();
    void scpiImportInvalidXml();
    void scpiImportFailFlashWrite();
    void scpiImportPassFlashWrite();
private:
    void setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory);

    std::unique_ptr<TestServerForSenseInterfaceCom5003> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_REGRESSION_ADJ_IMPORT_PERMISSION_COM5003_H