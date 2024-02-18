#include "mockdspinterface.h"
#include "notzeronumgen.h"

void MockDspInterface::fireActValInterrupt(QVector<float> actualValues)
{
    m_actualValues = actualValues;
    emit serverAnswer(0, 0, QVariant());
}

quint32 MockDspInterface::dataAcquisition(cDspMeasData *memgroup)
{
    memgroup->setData(m_actualValues);
    return NotZeroNumGen::getMsgNr();
}
