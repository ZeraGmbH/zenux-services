#ifndef TEST_REGRESSION_SENSE_INTERFACE_COM5003_H
#define TEST_REGRESSION_SENSE_INTERFACE_COM5003_H

#include "testserverforsenseinterfacecom5003.h"
#include "pcbinterface.h"
#include "resmanrunfacade.h"
#include <QObject>
#include <QStringList>
#include <QJsonObject>

class test_regression_sense_interface_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void checkVersionsOfSystemInterface();
    void checkChannelCatalogAsExpected();
    void checkRangesUL1();
    void checkRangesIL1();

    void constantRangeValuesIL3();
    void constantRangeValuesUL3();

    void channelAliasChangeOnREF();
    void mmodeCat();
    void invalidMode();
    void twiceSameMode();
private:
    QJsonObject loadJson(QString fileName);

    std::unique_ptr<TestServerForSenseInterfaceCom5003> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    static QStringList m_channelsExpectedAllOverThePlace;
    static QStringList m_rangesExpectedU;
    static QStringList m_rangesExpectedI;
};

#endif // TEST_REGRESSION_SENSE_INTERFACE_COM5003_H
