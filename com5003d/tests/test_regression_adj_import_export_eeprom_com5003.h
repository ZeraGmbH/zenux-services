#ifndef TEST_REGRESSION_ADJ_IMPORT_EXPORT_EEPROM_COM5003_H
#define TEST_REGRESSION_ADJ_IMPORT_EXPORT_EEPROM_COM5003_H

#include "mockforsenseinterfacecom5003.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_import_export_eeprom_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void directExportFlashNoMock();
    void directExportFlashGen();
    void directExportFlashCheckReference();
    void scpiWriteFlashInitial();

    void scpiWriteRandomFileAndFlashGen();
    void scpiWriteRandomFileFlashWriteFlashReadExportXmlAndCheck();

    // initial version saved invalid date time (up to ZENUX 4.4.3)
    void loadOriginalInvalidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck();
    void loadValidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck();

private:
    void setupServers();
    bool writeFile(QString filename, QByteArray data);
    QByteArray readFile(QString filename);

    std::unique_ptr<MockForSenseInterfaceCom5003> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_REGRESSION_ADJ_IMPORT_EXPORT_EEPROM_COM5003_H
