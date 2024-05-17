#ifndef TEST_ADJ_DATA_SERIALIZATION_H
#define TEST_ADJ_DATA_SERIALIZATION_H

#include "testserverforsenseinterfacemt310s2.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_adj_data_serialization : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void FromAdjToXMLFileForMt310s2();

private:
    void setupServers(QString deviceName, QString serialNum);

    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_proxyClient;

};

#endif // TEST_ADJ_DATA_SERIALIZATION_H
