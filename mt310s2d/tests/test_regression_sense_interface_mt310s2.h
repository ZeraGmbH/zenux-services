#ifndef TEST_REGRESSION_SENSE_INTERFACE_MT310S2_H
#define TEST_REGRESSION_SENSE_INTERFACE_MT310S2_H

#include <QObject>
#include <QStringList>

class test_regression_sense_interface_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void checkVersionsOfSystemInterface();
    void checkExportXml();
    void checkChannelCatalogAsExpected();
private:
    static QStringList m_channelsExpectedAllOverThePlace;
};

#endif // TEST_REGRESSION_SENSE_INTERFACE_MT310S2_H
