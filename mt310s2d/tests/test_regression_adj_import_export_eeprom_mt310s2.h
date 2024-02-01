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
    void init();
    void cleanup();

    void directExportFlashNoMock();
    void directExportFlashGen();
    void directExportFlashCheckReference();
    void scpiWriteFlashInitial();

    void scpiWriteRandomFileAndFlashGen();
    void scpiWriteRandomFileFlashWriteFlashReadExportXmlAndCheck();
    void loadRandomToEEpromWriteToFlashExportXmlAndCheck();

    void directExportFlashArbitraryVersionGen();
    void loadArbitraryVersionToEEprom();
    // all tests for clamps
private:
    void setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory);
    bool writeFile(QString filename, QByteArray data);
    QByteArray readFile(QString filename);

    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_proxyClient;
};

#endif // TEST_REGRESSION_ADJ_IMPORT_EXPORT_EEPROM_MT310S2_H
