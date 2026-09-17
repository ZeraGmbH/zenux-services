#ifndef TEST_REGRESSION_ADJ_IMPORT_EXPORT_EEPROM_MT310S2_H
#define TEST_REGRESSION_ADJ_IMPORT_EXPORT_EEPROM_MT310S2_H

#include "testserverforsenseinterfacemt310s2.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_import_export_eeprom_mt310s2 : public QObject
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
    void loadRandomToEEpromWriteToMemExportXmlAndCheck();

    void directExportMemArbitraryVersionGen();
    void loadArbitraryVersionToEEprom();

    void allClampsEEpromWrite_data();
    void allClampsEEpromWrite();

    void freshClampSetTypeIOnlyEepromCheck();
    void freshClampSetTypeUOnlyEepromCheck();
    void freshClampSetTypeUIEepromCheck();
private:
    void setupServers(const AbstractFactoryI2cCtrlPtr &ctrlFactory);

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    Zera::ProxyClientPtr m_proxyClient;
};

#endif // TEST_REGRESSION_ADJ_IMPORT_EXPORT_EEPROM_MT310S2_H
