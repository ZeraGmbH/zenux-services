#ifndef TEST_READ_VERSIONS_H
#define TEST_READ_VERSIONS_H

#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "pcbinterface.h"
#include "proxyclient.h"

class test_read_versions : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void readVersionNoEmob();
    void readVersionOneEmobChannelIAUX();
    void readVersionTwoEmobTwoChannels();

private:
    void setupServers();

    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;

};

#endif // TEST_READ_VERSIONS_H
