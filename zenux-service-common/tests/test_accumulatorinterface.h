#ifndef TEST_ACCUMULATORINTERFACE_H
#define TEST_ACCUMULATORINTERFACE_H

#include "accumulatorinterface.h"
#include <memory>

class test_accumulatorinterface : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase_data();
    void init();
    void cleanup();

    void findObjects();
    void readAccuStatus();
    void readAccuStateOfCharge();

private:
    std::shared_ptr<cSCPI> m_scpiInterface;
    AccumulatorInterface *m_accumulator;

    std::unique_ptr<Zera::XMLConfig::cReader> m_xmlConfigReader;
    std::unique_ptr<AccumulatorSettings> m_accuSettings;
};

#endif // TEST_ACCUMULATORINTERFACE_H
