#include "testdspinterface.h"
#include "notzeronumgen.h"

void TestDspInterface::fireActValInterrupt(QVector<float> actualValues)
{
    m_actualValues = actualValues;
    emit serverAnswer(0, 0, QVariant());
}

quint32 TestDspInterface::dataAcquisition(cDspMeasData *memgroup)
{
    memgroup->setData(m_actualValues);
    return NotZeroNumGen::getMsgNr();
}
