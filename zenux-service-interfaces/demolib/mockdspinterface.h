#ifndef MOCKDSPINTERFACE_H
#define MOCKDSPINTERFACE_H

#include "dspinterface.h"

class MockDspInterface : public Zera::cDSPInterface
{
    Q_OBJECT
public:
    void fireActValInterrupt(QVector<float> actualValues);
    quint32 dataAcquisition(cDspMeasData* memgroup) override;
private:
    QVector<float> m_actualValues;
};

#endif // MOCKDSPINTERFACE_H
