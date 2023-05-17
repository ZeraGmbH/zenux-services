#ifndef TEST_COM5003_CONFIG_LOAD_H
#define TEST_COM5003_CONFIG_LOAD_H

#include <QObject>

class test_com5003_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void fileLoaded();
};

#endif // TEST_COM5003_CONFIG_LOAD_H
