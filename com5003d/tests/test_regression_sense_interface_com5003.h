#ifndef TEST_REGRESSION_SENSE_INTERFACE_COM5003_H
#define TEST_REGRESSION_SENSE_INTERFACE_COM5003_H

#include "mockforsenseinterfacecom5003.h"
#include "pcbinterface.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_sense_interface_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void checkVersionsOfSystemInterface();
    void checkExportXml();
    void checkChannelCatalogAsExpected();
    void checkRangesUL1();
    void checkRangesIL1();
private:
    std::unique_ptr<MockForSenseInterfaceCom5003> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    static QStringList m_channelsExpectedAllOverThePlace;
    static QStringList m_rangesExpectedU;
    static QStringList m_rangesExpectedI;
};

#endif // TEST_REGRESSION_SENSE_INTERFACE_COM5003_H
