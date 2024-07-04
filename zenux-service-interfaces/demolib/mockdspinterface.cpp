#include "mockdspinterface.h"
#include "notzeronumgen.h"
#include "reply.h"

void MockDspInterface::fireActValInterrupt(QVector<float> actualValues, int irqNo)
{
    m_actualValues = actualValues;
    // Dsp sends DSPINT:%1. Example clients do not seem to care about
    // 'DSPINT': they look just for irq number (after ':'). See how this
    // evolves once working on SEC.
    QString message = QString("DSPINT:%1").arg(irqNo);
    constexpr quint32 messageNum = 0; // async interrupt
    emit serverAnswer(messageNum, ack, message);
}

quint32 MockDspInterface::dataAcquisition(cDspMeasData *memgroup)
{
    memgroup->setData(m_actualValues);
    return sendCmdResponse("");
}

quint32 MockDspInterface::dspMemoryWrite(cDspMeasData *memgroup)
{
    emit sigDspMemoryWrite(memgroup->getName(), memgroup->getData());
    return sendCmdResponse("");
}

quint32 MockDspInterface::activateInterface()
{
    emit sigDspStarted();
    return sendCmdResponse("");
}

quint32 MockDspInterface::readMaximumLoad()
{
    return sendCmdResponse("BUSYMAX:42.0");
}

quint32 MockDspInterface::resetMaximumLoad()
{
    return sendCmdResponse("");
}

quint32 MockDspInterface::sendCmdResponse(QString answer)
{
    quint32 messageNum = NotZeroNumGen::getMsgNr();
    QMetaObject::invokeMethod(this,
                              "serverAnswer",
                              Qt::QueuedConnection,
                              Q_ARG(quint32, messageNum),
                              Q_ARG(quint8, ack),
                              Q_ARG(QVariant, QVariant(answer))
                              );
    return messageNum;
}
