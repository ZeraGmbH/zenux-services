#ifndef TEST_MT310S2_CONFIG_LOAD_H
#define TEST_MT310S2_CONFIG_LOAD_H

#include <QObject>

class test_config_load_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void fileLoaded();
};

#endif // TEST_MT310S2_CONFIG_LOAD_H
