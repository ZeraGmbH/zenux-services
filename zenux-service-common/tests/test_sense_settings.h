#ifndef TEST_SENSE_SETTINGS_H
#define TEST_SENSE_SETTINGS_H

#include <QObject>

class test_sense_settings : public QObject
{
    Q_OBJECT
private slots:
    void findByAlias1Com5003();
    void findByAlias1Mt310s2();
    void findByInMxCom5003();
};

#endif // TEST_SENSE_SETTINGS_H
