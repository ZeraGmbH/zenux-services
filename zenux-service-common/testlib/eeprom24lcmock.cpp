#include "eeprom24lcmock.h"

static constexpr int sizeFlash = 32768;

QHash<QString, QHash<short, QByteArray>> EEprom24LCMock::m_flashData;
QHash<QString, QHash<short, int>>        EEprom24LCMock::m_flashDataWriteCounts;
QHash<QString, QHash<short, bool>>       EEprom24LCMock::m_returnReducedDataSizeOnRead;

EEprom24LCMock::EEprom24LCMock(QString devNode, short i2cAddr) :
    m_devNode(devNode),
    m_i2cAddr(i2cAddr)
{
    if(m_flashData.contains(devNode))
        return;
    if(m_flashData[devNode].contains(i2cAddr))
        return;
    doReset(sizeFlash);
}

int EEprom24LCMock::WriteData(char *data, ushort count, ushort adr)
{
    if(adr != 0)
        qFatal("Address other than 0 is not yet supported!");
    if(count > sizeFlash)
        qFatal("Cannot write data of length %i / max is %i", count, sizeFlash);

    doReset(count);
    QByteArray &flashEntry = m_flashData[m_devNode][m_i2cAddr];
    for(int i=0; i<count; i++)
        flashEntry[i] = data[i];
    m_flashDataWriteCounts[m_devNode][m_i2cAddr]++;
    return count;
}

int EEprom24LCMock::ReadData(char *data, ushort count, ushort adr)
{
    if(adr != 0)
        qFatal("Address other than 0 is not yet supported!");
    if(count > sizeFlash)
        qFatal("Cannot read data of length %i / max is %i", count, sizeFlash);

    const QByteArray flashEntry = m_flashData[m_devNode][m_i2cAddr];
    ushort reducedCount = std::min(count, ushort(flashEntry.size()));
    for(int i=0; i<reducedCount; i++)
        data[i] = flashEntry[i];
    bool reduceCount = m_returnReducedDataSizeOnRead[m_devNode][m_i2cAddr];
    return reduceCount ? reducedCount: count;
}

int EEprom24LCMock::Reset()
{
    doReset(0);
    return size();
}

int EEprom24LCMock::size()
{
    return sizeFlash;
}

void EEprom24LCMock::returnReduceCountOnErrorRead()
{
    m_returnReducedDataSizeOnRead[m_devNode][m_i2cAddr] = true;
}

void EEprom24LCMock::cleanAll()
{
    m_flashData.clear();
    m_flashDataWriteCounts.clear();
    m_returnReducedDataSizeOnRead.clear();
}

QByteArray EEprom24LCMock::getData(QString devNode, short adr)
{
    QByteArray ret;
    if(m_flashData.contains(devNode))
        if(m_flashData[devNode].contains(adr))
            ret = m_flashData[devNode][adr];
    return ret;
}

int EEprom24LCMock::getWriteCount(QString devNode, short adr)
{
    return m_flashDataWriteCounts[devNode][adr];
}

void EEprom24LCMock::doReset(int size)
{
    m_flashData[m_devNode][m_i2cAddr] = QByteArray(size, 0xff);
}
