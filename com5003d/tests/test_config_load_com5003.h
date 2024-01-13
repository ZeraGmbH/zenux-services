#ifndef TEST_COM5003_CONFIG_LOAD_H
#define TEST_COM5003_CONFIG_LOAD_H

#include <QObject>

class test_config_load_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void fileLoaded();
};

#endif // TEST_COM5003_CONFIG_LOAD_H
