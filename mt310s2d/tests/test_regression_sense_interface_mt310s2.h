#ifndef TEST_REGRESSION_SENSE_INTERFACE_MT310S2_H
#define TEST_REGRESSION_SENSE_INTERFACE_MT310S2_H

#include <QObject>
#include <QStringList>

class test_regression_sense_interface_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void checkVersionsOfSystemInterface();
    void checkExportXml();
    void checkChannelCatalogAsExpected();
    void checkRangesUL1();
    void checkRangesIL1();
    void addClampIL1_CL120A();
    void addClampIL2_CL800ADC1000VDC();
    void addRemoveClampILAUX_CL800ADC1000VDC();
private:
    static QStringList m_channelsExpectedAllOverThePlace;
    static QStringList m_rangesExpectedU;
    static QStringList m_rangesExpectedI;
    static QStringList m_rangesExpectedI_CL120A;
    static QStringList m_rangesExpectedU_CL800ADC1000VDC;
    static QStringList m_rangesExpectedI_CL800ADC1000VDC;
};

#endif // TEST_REGRESSION_SENSE_INTERFACE_MT310S2_H
