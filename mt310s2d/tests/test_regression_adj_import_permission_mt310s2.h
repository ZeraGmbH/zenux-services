#ifndef TEST_REGRESSION_ADJ_IMPORT_PERMISSION_MT310S2_H
#define TEST_REGRESSION_ADJ_IMPORT_PERMISSION_MT310S2_H

#include "testserverforsenseinterfacemt310s2.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_import_permission_mt310s2 : public QObject
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

    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
};

#endif // TEST_REGRESSION_ADJ_IMPORT_PERMISSION_MT310S2_H
