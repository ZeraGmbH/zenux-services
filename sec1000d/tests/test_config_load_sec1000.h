#ifndef TEST_SEC1000_CONFIG_LOAD_H
#define TEST_SEC1000_CONFIG_LOAD_H

#include <QObject>

class test_config_load_sec1000 : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void fileLoaded();
};

#endif // TEST_SEC1000_CONFIG_LOAD_H
