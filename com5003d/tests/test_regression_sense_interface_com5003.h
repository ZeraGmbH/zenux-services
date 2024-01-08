#ifndef TEST_REGRESSION_SENSE_INTERFACE_COM5003_H
#define TEST_REGRESSION_SENSE_INTERFACE_COM5003_H

#include "mockforsenseinterfacecom5003.h"
#include "pcbinterface.h"
#include "resmanrunfacade.h"
#include <QObject>
#include <QStringList>
#include <QJsonObject>

class test_regression_sense_interface_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void checkVersionsOfSystemInterface();
    void checkChannelCatalogAsExpected();
    void checkRangesUL1();
    void checkRangesIL1();

    // pairs of tests:
    // * ...GenJson: Log generated json - not a real test just in case we
    //   change behavior by will
    // * ...Check: Compare to references stored in 'regression-data' resources
    void constantRangeValuesIL3GenJson();
    void constantRangeValuesIL3Check();
    void constantRangeValuesUL3GenJson();
    void constantRangeValuesUL3Check();
private:
    QJsonObject loadJson(QString fileName);

    std::unique_ptr<MockForSenseInterfaceCom5003> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    static QStringList m_channelsExpectedAllOverThePlace;
    static QStringList m_rangesExpectedU;
    static QStringList m_rangesExpectedI;
};

#endif // TEST_REGRESSION_SENSE_INTERFACE_COM5003_H
