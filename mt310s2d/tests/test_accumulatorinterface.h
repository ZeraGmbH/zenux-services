#ifndef TEST_ACCUMULATORINTERFACE_H
#define TEST_ACCUMULATORINTERFACE_H

#include <QObject>
#include <memory>
#include "accumulatorinterface.h"

class test_accumulatorinterface : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void findObject();
    void readAccumulatorStatus();

private:
    std::unique_ptr<cSCPI> m_scpiInterface;
    AccumulatorInterface *m_accumulator;
};

#endif // TEST_ACCUMULATORINTERFACE_H
