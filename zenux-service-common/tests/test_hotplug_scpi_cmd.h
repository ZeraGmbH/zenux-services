#ifndef TEST_HOTPLUG_SCPI_CMD_H
#define TEST_HOTPLUG_SCPI_CMD_H

#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "pcbinterface.h"
#include "proxyclient.h"

class test_hotplug_scpi_cmd : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void readErrorStatusNoParamNoEmob();
    void readErrorStatusNoParamEmobIAUX();
    void readErrorStatusIAUXEmobI3IAUX();
    void readErrorStatusI3EmobIAUX();

    void clearErrorStatusNoParamNoEmob();
    void clearErrorStatusNoParamEmobIAUX();
    void clearErrorStatusIAUXEmobI3IAUX();
    void clearErrorStatusI3EmobIAUX();

    void readDataEmobIL1();
    void readDataEmobIL1MissingParam();
    void readDataEmobIL1NotAvail();

    void writeDataEmobIL1Id1();
    void writeDataEmobIL1Id1Empty();
    void writeDataEmobIL1Id1EmptyDoubleSemicolon();
    void writeDataEmobIL1Id3();
    void writeDataEmobIL1Id4Invalid();
    void writeDataEmobMissingParam();
    void writeDataEmobIL1NotAvail();

private:
    void setupServers();
    QByteArray createTestEmobExchangeData();

    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;

};

#endif // TEST_HOTPLUG_SCPI_CMD_H
