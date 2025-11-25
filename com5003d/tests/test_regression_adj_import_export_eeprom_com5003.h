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
    void cleanup();

    void directExportFlashError();
    void directExportFlashGen();
    void directExportFlashCheckReference();
    void scpiWriteFlashInitial();

    void scpiWriteRandomFileAndFlashGen();
    void scpiWriteRandomFileFlashWriteFlashReadExportXmlAndCheck();

    // initial version saved invalid date time (up to ZENUX 4.4.3)
    void loadOriginalInvalidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck();
    void loadValidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck();

    void directExportFlashArbitraryVersionGen();
    void loadArbitraryVersionToEEprom();

private:
    void setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory);

    std::unique_ptr<TestServerForSenseInterfaceCom5003> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_proxyClient;
};

#endif // TEST_REGRESSION_ADJ_IMPORT_EXPORT_EEPROM_COM5003_H
