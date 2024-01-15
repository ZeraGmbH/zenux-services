#ifndef TEST_ADJ_DENY_EEPROM_IMPORT_COM5003_H
#define TEST_ADJ_DENY_EEPROM_IMPORT_COM5003_H

#include "mockforsenseinterfacecom5003.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_adj_deny_import_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void loadEEpromWithStoredNamesAndVersions();
    void loadEEpromAndDenyDifferentDeviceName();

private:
    void setupServers();
    QByteArray readFile(QString filename);

    std::unique_ptr<MockForSenseInterfaceCom5003> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_ADJ_DENY_EEPROM_IMPORT_COM5003_H