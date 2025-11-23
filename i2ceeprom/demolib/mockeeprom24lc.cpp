#include "mockeeprom24lc.h"
#include <QFile>

QHash<EepromWithMuxParams, QByteArray> MockEEprom24LC::m_flashData;
QHash<EepromWithMuxParams, int>        MockEEprom24LC::m_flashDataReadCounts;
QHash<EepromWithMuxParams, int>        MockEEprom24LC::m_flashDataWriteCounts;
QHash<EepromWithMuxParams, bool>       MockEEprom24LC::m_returnReducedDataSizeOnRead;

MockEEprom24LC::MockEEprom24LC(const I2cAddressParameter &i2cAddressParam, int byteCapacity,
                               const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo) :
    m_i2cAddress(i2cAddressParam),
    m_byteCapacity(byteCapacity),
    m_i2cAddressMux(i2cAddressMux),
    m_muxChannelNo(muxChannelNo)
{
    if(m_flashData.contains(getFullParams()))
        return;
    doReset(byteCapacity);
}

int MockEEprom24LC::WriteData(char *data, ushort count, ushort adr)
{
    if(adr != 0)
        qFatal("Address other than 0 is not yet supported!");
    if(count > getByteSize())
        qFatal("Cannot write data of length %i / max is %i", count, getByteSize());

    doReset(count);
    QByteArray &flashEntry = m_flashData[getFullParams()];
    for(int i=0; i<count; i++)
        flashEntry[i] = data[i];
    m_flashDataWriteCounts[getFullParams()]++;
    return count;
}

int MockEEprom24LC::ReadData(char *data, ushort count, ushort adr)
{
    if(adr != 0)
        qFatal("Address other than 0 is not yet supported!");
    if(count > getByteSize())
        qFatal("Cannot read data of length %i / max is %i", count, getByteSize());

    const QByteArray flashEntry = m_flashData[getFullParams()];
    ushort reducedCount = std::min(count, ushort(flashEntry.size()));
    for(int i=0; i<reducedCount; i++)
        data[i] = flashEntry[i];
    m_flashDataReadCounts[getFullParams()]++;
    bool reduceCount = m_returnReducedDataSizeOnRead[getFullParams()];
    return reduceCount ? reducedCount: count;
}

int MockEEprom24LC::Reset()
{
    doReset(0);
    return getByteSize();
}

int MockEEprom24LC::getByteSize() const
{
    return m_byteCapacity;
}

void MockEEprom24LC::returnReduceCountOnErrorRead()
{
    m_returnReducedDataSizeOnRead[getFullParams()] = true;
}

void MockEEprom24LC::mockCleanAll()
{
    m_flashData.clear();
    m_flashDataReadCounts.clear();
    m_flashDataWriteCounts.clear();
    m_returnReducedDataSizeOnRead.clear();
}

QByteArray MockEEprom24LC::mockGetData(const I2cAddressParameter &i2cAddressParam,
                                       const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    EepromWithMuxParams params{i2cAddressParam, i2cAddressMux, muxChannelNo};
    QByteArray ret;
    if(m_flashData.contains(params))
        ret = m_flashData[params];
    return ret;
}

void MockEEprom24LC::mockSetData(const I2cAddressParameter &i2cAddressParam, QByteArray data,
                                 const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    EepromWithMuxParams params{i2cAddressParam, i2cAddressMux, muxChannelNo};
    m_flashData[params] = data;
}

int MockEEprom24LC::mockGetReadCount(const I2cAddressParameter &i2cAddressParam,
                                     const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    EepromWithMuxParams params{i2cAddressParam, i2cAddressMux, muxChannelNo};
    return m_flashDataReadCounts[params];
}

int MockEEprom24LC::mockGetWriteCount(const I2cAddressParameter &i2cAddressParam,
                                      const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    EepromWithMuxParams params{i2cAddressParam, i2cAddressMux, muxChannelNo};
    return m_flashDataWriteCounts[params];
}

bool MockEEprom24LC::mockWriteToFile(const I2cAddressParameter &i2cAddressParam,
                                     QString fileName,
                                     const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    QByteArray data = mockGetData(i2cAddressParam, i2cAddressMux, muxChannelNo);
    if(data.isEmpty())
        return false;
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly))
        return file.write(data) == data.length();
    return false;
}

bool MockEEprom24LC::mockReadFromFile(const I2cAddressParameter &i2cAddressParam, QString fileName,
                                      const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        if(data.isEmpty())
            return false;
        mockSetData(i2cAddressParam, data, i2cAddressMux, muxChannelNo);
        return true;
    }
    return false;
}

bool MockEEprom24LC::mockCompareWithFile(const I2cAddressParameter &i2cAddressParam, QString fileName,
                                         const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        if(data.isEmpty()) // for now...
            return false;
        return data == mockGetData(i2cAddressParam, i2cAddressMux, muxChannelNo);
    }
    return false;
}

void MockEEprom24LC::doReset(int size)
{
    m_flashData[getFullParams()] = QByteArray(size, 0xff);
}

EepromWithMuxParams MockEEprom24LC::getFullParams() const
{
    return {m_i2cAddress, m_i2cAddressMux, m_muxChannelNo};
}
