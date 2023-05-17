#ifndef TEST_ZDSP1_CONFIG_LOAD_H
#define TEST_ZDSP1_CONFIG_LOAD_H

#include <QObject>

class test_zdsp1_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void fileLoaded();
};

#endif // TEST_ZDSP1_CONFIG_LOAD_H
