#include "mockeeprom24lc.h"

static constexpr int sizeFlash = 32768;

QHash<QString, QHash<short, QByteArray>> MockEEprom24LC::m_flashData;
QHash<QString, QHash<short, int>>        MockEEprom24LC::m_flashDataWriteCounts;
QHash<QString, QHash<short, bool>>       MockEEprom24LC::m_returnReducedDataSizeOnRead;

MockEEprom24LC::MockEEprom24LC(QString devNode, short i2cAddr) :
    m_devNode(devNode),
    m_i2cAddr(i2cAddr)
{
    if(m_flashData.contains(devNode))
        return;
    if(m_flashData[devNode].contains(i2cAddr))
        return;
    doReset(sizeFlash);
}

int MockEEprom24LC::WriteData(char *data, ushort count, ushort adr)
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

int MockEEprom24LC::ReadData(char *data, ushort count, ushort adr)
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

int MockEEprom24LC::Reset()
{
    doReset(0);
    return size();
}

int MockEEprom24LC::size()
{
    return sizeFlash;
}

void MockEEprom24LC::returnReduceCountOnErrorRead()
{
    m_returnReducedDataSizeOnRead[m_devNode][m_i2cAddr] = true;
}

void MockEEprom24LC::cleanAll()
{
    m_flashData.clear();
    m_flashDataWriteCounts.clear();
    m_returnReducedDataSizeOnRead.clear();
}

QByteArray MockEEprom24LC::getData(QString devNode, short adr)
{
    QByteArray ret;
    if(m_flashData.contains(devNode))
        if(m_flashData[devNode].contains(adr))
            ret = m_flashData[devNode][adr];
    return ret;
}

int MockEEprom24LC::getWriteCount(QString devNode, short adr)
{
    return m_flashDataWriteCounts[devNode][adr];
}

void MockEEprom24LC::doReset(int size)
{
    m_flashData[m_devNode][m_i2cAddr] = QByteArray(size, 0xff);
}
