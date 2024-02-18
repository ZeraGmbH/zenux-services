#include "mockdspinterface.h"
#include "notzeronumgen.h"

void MockDspInterface::fireActValInterrupt(QVector<float> actualValues, int irqNo)
{
    m_actualValues = actualValues;
    // Dsp sends DSPINT:%1. Example clients do not seem to care about
    // 'DSPINT': they look just for irq number (after ':'). See how this
    // evolves once working on SEC.
    QString message = QString("DSPINT:%1").arg(irqNo);
    emit serverAnswer(0, 0, message);
}

quint32 MockDspInterface::dataAcquisition(cDspMeasData *memgroup)
{
    memgroup->setData(m_actualValues);
    return NotZeroNumGen::getMsgNr();
}
