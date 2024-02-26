#ifndef MOCKDSPINTERFACE_H
#define MOCKDSPINTERFACE_H

#include "dspinterface.h"

class MockDspInterface : public Zera::cDSPInterface
{
    Q_OBJECT
public:
    void fireActValInterrupt(QVector<float> actualValues, int irqNo);
    quint32 dataAcquisition(cDspMeasData* memgroup) override;
    quint32 dspMemoryWrite(cDspMeasData* memgroup) override;
signals:
    void sigDspMemoryWrite(QString name, QVector<float> values);
private:
    quint32 sendCmdResponse();
    QVector<float> m_actualValues;
};

#endif // MOCKDSPINTERFACE_H
