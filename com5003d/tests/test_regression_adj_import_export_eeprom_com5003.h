#ifndef TEST_REGRESSION_ADJ_IMPORT_EXPORT_EEPROM_COM5003_H
#define TEST_REGRESSION_ADJ_IMPORT_EXPORT_EEPROM_COM5003_H

#include "testserverforsenseinterfacecom5003.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_import_export_eeprom_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();

    void directExportMemError();
    void directExportMemGen();
    void directExportMemCheckReference();
    void scpiWriteMemInitial();

    void scpiWriteRandomFileAndMemGen();
    void scpiWriteRandomFileMemWriteMemReadExportXmlAndCheck();

    // initial version saved invalid date time (up to ZENUX 4.4.3)
    void loadOriginalInvalidDateTimeRandomToMemExportXmlAndCheck();
    void loadValidDateTimeRandomToMemWriteToMemExportXmlAndCheck();

    void directExportMemArbitraryVersionGen();
    void loadArbitraryVersionToEEprom();

private:
    void setupServers(const AbstractFactoryI2cCtrlPtr &ctrlFactory);

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<TestServerForSenseInterfaceCom5003> m_testServer;
    Zera::ProxyClientPtr m_proxyClient;
};

#endif // TEST_REGRESSION_ADJ_IMPORT_EXPORT_EEPROM_COM5003_H
