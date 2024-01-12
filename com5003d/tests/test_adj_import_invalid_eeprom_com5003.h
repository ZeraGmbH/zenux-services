#ifndef TEST_ADJ_IMPORT_INVALID_EEPROM_COM5003_H
#define TEST_ADJ_IMPORT_INVALID_EEPROM_COM5003_H

#include "mockforsenseinterfacecom5003.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

// While making we learned that there is a solid checksum test in AdjustmentEeprom
// So close to AdjustmentEeprom might be a better place for this. and that's why
// there is no Mt310s2 twin for this.
class test_adj_import_invalid_eeprom_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void importEmptyData();
    void importIncompleteData();

private:
    void setupServers();
    bool writeFile(QString filename, QByteArray data);
    QByteArray readFile(QString filename);

    std::unique_ptr<MockForSenseInterfaceCom5003> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_ADJ_IMPORT_INVALID_EEPROM_COM5003_H
