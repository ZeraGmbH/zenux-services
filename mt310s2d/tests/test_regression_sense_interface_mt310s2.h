#ifndef TEST_REGRESSION_SENSE_INTERFACE_MT310S2_H
#define TEST_REGRESSION_SENSE_INTERFACE_MT310S2_H

#include "mockforsenseinterfacemt310s2.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>
#include <QStringList>
#include <QJsonObject>

class test_regression_sense_interface_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void checkVersionsOfSystemInterface();
    void checkExportXml();
    void checkChannelCatalogAsExpected();
    void checkRangesUL1();
    void checkRangesIL1();
    void addClampIL1_CL120A();
    void addClampIL2_CL800ADC1000VDC();
    void addRemoveClampIAUX_CL800ADC1000VDC();
    void genJsonRejectionValuesAllClampsIL3();
    void genJsonRejectionValuesAllClampsIAUX();
    void genJsonRejectionValuesAllClampsUAUX();
private:
    void addClamp(int clampType, QString channelAlias1);
    void removeAllClamps();
    void genJsonConstantValuesAllRanges(QString channelName, QString channelNameAdRemoveClamps = "");

    std::unique_ptr<MockForSenseInterfaceMt310s2> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    static QStringList m_channelsExpectedAllOverThePlace;
    static QStringList m_rangesExpectedU;
    static QStringList m_rangesExpectedI;
    static QStringList m_rangesExpectedI_Internal;
    static QStringList m_rangesExpectedI_DummyAux;
    static QStringList m_rangesExpectedI_CL120A;
    static QStringList m_rangesExpectedU_CL800ADC1000VDC;
    static QStringList m_rangesExpectedI_CL800ADC1000VDC;
};

#endif // TEST_REGRESSION_SENSE_INTERFACE_MT310S2_H
