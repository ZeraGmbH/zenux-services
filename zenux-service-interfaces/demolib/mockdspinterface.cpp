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
    quint32 messageNum = NotZeroNumGen::getMsgNr();
    quint8 reply = 0;
    QVariant answer = QString();
    QMetaObject::invokeMethod(this,
                              "serverAnswer",
                              Qt::QueuedConnection,
                              Q_ARG(quint32, messageNum),
                              Q_ARG(quint8, reply),
                              Q_ARG(QVariant, answer)
                              );
    return messageNum;
}
