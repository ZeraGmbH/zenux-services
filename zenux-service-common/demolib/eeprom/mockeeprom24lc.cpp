#include "mockeeprom24lc.h"
#include <QFile>

QHash<QString, QHash<short, QByteArray>> MockEEprom24LC::m_flashData;
QHash<QString, QHash<short, int>>        MockEEprom24LC::m_flashDataReadCounts;
QHash<QString, QHash<short, int>>        MockEEprom24LC::m_flashDataWriteCounts;
QHash<QString, QHash<short, bool>>       MockEEprom24LC::m_returnReducedDataSizeOnRead;

MockEEprom24LC::MockEEprom24LC(QString devNode, short i2cAddr, int byteCapacity) :
    EepromI2cDeviceInterface(byteCapacity),
    m_devNode(devNode),
    m_i2cAddr(i2cAddr)
{
    if(m_flashData.contains(devNode))
        return;
    if(m_flashData[devNode].contains(i2cAddr))
        return;
    doReset(getByteSize());
}

int MockEEprom24LC::WriteData(char *data, ushort count, ushort adr)
{
    if(adr != 0)
        qFatal("Address other than 0 is not yet supported!");
    if(count > getByteSize())
        qFatal("Cannot write data of length %i / max is %i", count, getByteSize());

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
    if(count > getByteSize())
        qFatal("Cannot read data of length %i / max is %i", count, getByteSize());

    const QByteArray flashEntry = m_flashData[m_devNode][m_i2cAddr];
    ushort reducedCount = std::min(count, ushort(flashEntry.size()));
    for(int i=0; i<reducedCount; i++)
        data[i] = flashEntry[i];
    m_flashDataReadCounts[m_devNode][m_i2cAddr]++;
    bool reduceCount = m_returnReducedDataSizeOnRead[m_devNode][m_i2cAddr];
    return reduceCount ? reducedCount: count;
}

int MockEEprom24LC::Reset()
{
    doReset(0);
    return getByteSize();
}

void MockEEprom24LC::returnReduceCountOnErrorRead()
{
    m_returnReducedDataSizeOnRead[m_devNode][m_i2cAddr] = true;
}

void MockEEprom24LC::mockCleanAll()
{
    m_flashData.clear();
    m_flashDataReadCounts.clear();
    m_flashDataWriteCounts.clear();
    m_returnReducedDataSizeOnRead.clear();
}

QByteArray MockEEprom24LC::mockGetData(QString devNode, short adr)
{
    QByteArray ret;
    if(m_flashData.contains(devNode))
        if(m_flashData[devNode].contains(adr))
            ret = m_flashData[devNode][adr];
    return ret;
}

void MockEEprom24LC::mockSetData(QString devNode, short adr, QByteArray data)
{
    m_flashData[devNode][adr] = data;
}

int MockEEprom24LC::mockGetReadCount(QString devNode, short adr)
{
    return m_flashDataReadCounts[devNode][adr];
}

int MockEEprom24LC::mockGetWriteCount(QString devNode, short adr)
{
    return m_flashDataWriteCounts[devNode][adr];
}

bool MockEEprom24LC::mockWriteToFile(QString devNode, short adr, QString fileName)
{
    QByteArray data = mockGetData(devNode, adr);
    if(data.isEmpty())
        return false;
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly))
        return file.write(data) == data.length();
    return false;
}

bool MockEEprom24LC::mockReadFromFile(QString devNode, short adr, QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        if(data.isEmpty())
            return false;
        mockSetData(devNode, adr, data);
        return true;
    }
    return false;
}

bool MockEEprom24LC::mockCompareWithFile(QString devNode, short adr, QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        if(data.isEmpty()) // for now...
            return false;
        return data == mockGetData(devNode, adr);
    }
    return false;
}

void MockEEprom24LC::doReset(int size)
{
    m_flashData[m_devNode][m_i2cAddr] = QByteArray(size, 0xff);
}
