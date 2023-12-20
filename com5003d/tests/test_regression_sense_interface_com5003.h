#ifndef TEST_REGRESSION_SENSE_INTERFACE_COM5003_H
#define TEST_REGRESSION_SENSE_INTERFACE_COM5003_H

#include <QObject>

class test_regression_sense_interface_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void checkVersionsOfSystemInterface();
    void checkExportXml();
    void checkChannelCatalogAsExpected();
private:
    static QStringList m_channelsExpectedAllOverThePlace;
};

#endif // TEST_REGRESSION_SENSE_INTERFACE_COM5003_H
