#ifndef TESTDSPINTERFACE_H
#define TESTDSPINTERFACE_H

#include "dspinterface.h"

class TestDspInterface : public Zera::cDSPInterface
{
    Q_OBJECT
public:
    void fireActValInterrupt(QVector<float> actualValues);
    quint32 dataAcquisition(cDspMeasData* memgroup) override;
private:
    QVector<float> m_actualValues;
};

#endif // TESTDSPINTERFACE_H
