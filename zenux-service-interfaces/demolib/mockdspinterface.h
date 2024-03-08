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
    quint32 activateInterface() override;
signals:
    void sigDspMemoryWrite(QString name, QVector<float> values);
    void sigDspStarted();
private:
    quint32 sendCmdResponse();
    QVector<float> m_actualValues;
};

typedef std::shared_ptr<MockDspInterface> MockDspInterfacePtr;

#endif // MOCKDSPINTERFACE_H
