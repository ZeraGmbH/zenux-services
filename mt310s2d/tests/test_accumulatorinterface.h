#ifndef TEST_ACCUMULATORINTERFACE_H
#define TEST_ACCUMULATORINTERFACE_H

#include "accumulatorinterface.h"
#include "atmelsyscntrltest.h"
#include <memory>

class test_accumulatorinterface : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void findObjects();
    void readAccumulatorStatus();
    void readAccumulatorSoc();

private:
    std::unique_ptr<cSCPI> m_scpiInterface;
    AccumulatorInterface *m_accumulator;
    std::unique_ptr<AtmelSysCntrlTest> m_atmelSysCntrl;

    std::unique_ptr<Zera::XMLConfig::cReader> m_xmlConfigReader;
    std::unique_ptr<AccumulatorSettings> m_accuSettings;
};

#endif // TEST_ACCUMULATORINTERFACE_H
