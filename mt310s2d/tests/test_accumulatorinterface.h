#ifndef TEST_ACCUMULATORINTERFACE_H
#define TEST_ACCUMULATORINTERFACE_H

#include "accumulatorinterface.h"
#include "atmelsyscntrltest.h"
#include <QObject>
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
};

#endif // TEST_ACCUMULATORINTERFACE_H
