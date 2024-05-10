#ifndef TEST_ADJ_DENY_EEPROM_IMPORT_COM5003_H
#define TEST_ADJ_DENY_EEPROM_IMPORT_COM5003_H

#include "testserverforsenseinterfacecom5003.h"
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

    std::unique_ptr<TestServerForSenseInterfaceCom5003> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_proxyClient;
};

#endif // TEST_ADJ_DENY_EEPROM_IMPORT_COM5003_H
