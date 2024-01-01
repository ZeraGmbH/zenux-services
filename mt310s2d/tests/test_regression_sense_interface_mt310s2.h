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
    void checkChannelCatalogAsExpected();
    void checkRangesUL1();
    void checkRangesIL1();
    void addClampIL1_CL120A();
    void addClampIL2_CL800ADC1000VDC();
    void addRemoveClampIAUX_CL800ADC1000VDC();

    // pairs of tests:
    // * ...GenJson: Log generated json - not a real test just in case we
    //   change behavior by will
    // * ...Check: Compare to references stored in 'regression-data' resources
    void clampIdsNamesGenJson();
    void clampIdsNamesCheck();

    // without clamps
    void constantRangeValuesIL3GenJson();
    void constantRangeValuesIL3Check();
    void constantRangeValuesUL3GenJson();
    void constantRangeValuesUL3Check();
    void constantRangeValuesIAUXGenJson();
    void constantRangeValuesIAUXCheck();
    void constantRangeValuesUAUXGenJson();
    void constantRangeValuesUAUXCheck();

    // without clamps / ADJ mode
    void constantRangeValuesIL3ModeAdjGenJson();
    void constantRangeValuesIL3ModeAdjCheck();
    void constantRangeValuesIAUXModeAdjGenJson();
    void constantRangeValuesIAUXModeAdjCheck();

    // without clamps / HF mode (what is that?? it just makes all ranges unavailable)
    void constantRangeValuesIL3ModeHfGenJson();
    void constantRangeValuesIL3ModeHfCheck();
    void constantRangeValuesIAUXModeHfGenJson();
    void constantRangeValuesIAUXModeHfCheck();

    // with all clamps
    void constantRangeValuesAllClampsIL3GenJson();
    void constantRangeValuesAllClampsIL3Check();
    void constantRangeValuesAllClampsIAUXGenJson();
    void constantRangeValuesAllClampsIAUXCheck();
    void constantRangeValuesAllClampsUAUXGenJson();
    void constantRangeValuesAllClampsUAUXCheck();
private:
    void addClamp(int clampTypeNo, QString channelAlias1);
    void removeAllClamps();
    void genJsonConstantValuesAllRangesForAllClamps(QString channelName, QString channelNameAdRemoveClamps);
    bool checkJsonConstantValuesAllRangesForAllClamps(QJsonObject jsonReference, QString channelName, QString channelNameAdRemoveClamps);
    QJsonObject loadJson(QString fileName);

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
