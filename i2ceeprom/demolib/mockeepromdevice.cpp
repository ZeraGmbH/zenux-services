#include "mockeepromdevice.h"
#include <QFile>

QHash<EepromWithMuxParams, QByteArray> MockEepromDevice::m_memData;
QHash<EepromWithMuxParams, int>        MockEepromDevice::m_memDataReadCounts;
QHash<EepromWithMuxParams, int>        MockEepromDevice::m_memDataWriteCounts;
bool                                   MockEepromDevice::m_globalError = false;

MockEepromDevice::MockEepromDevice(const I2cAddressParameter &i2cAddressParam, int byteCapacity,
                                   const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo) :
    m_i2cAddress(i2cAddressParam),
    m_byteCapacity(byteCapacity),
    m_i2cAddressMux(i2cAddressMux),
    m_muxChannelNo(muxChannelNo)
{
    EepromWithMuxParams fullParams = getFullParams();
    if(m_memData.contains(fullParams))
        return;
    // Internal memory is assumed plugged and can be 'unplugged' by setGlobalError
    if(m_muxChannelNo == InvalidMux)
        doReset(byteCapacity);
}

bool MockEepromDevice::isMemoryPlugged() const
{
    if (m_globalError)
        return false;
    return m_memData.contains(getFullParams());
}

int MockEepromDevice::WriteData(char *data, ushort count, ushort adr)
{
    if (m_globalError)
        return 0;
    if(adr != 0)
        qFatal("Address other than 0 is not yet supported!");
    if(count > getByteSize())
        qFatal("Cannot write data of length %i / max is %i", count, getByteSize());

    doReset(count);
    QByteArray &flashEntry = m_memData[getFullParams()];
    for(int i=0; i<count; i++)
        flashEntry[i] = data[i];
    m_memDataWriteCounts[getFullParams()]++;
    return count;
}

int MockEepromDevice::ReadData(char *data, ushort count, ushort adr)
{
    if (m_globalError)
        return 0;
    if(adr != 0)
        qFatal("Address other than 0 is not yet supported!");
    if(count > getByteSize())
        qFatal("Cannot read data of length %i / max is %i", count, getByteSize());

    const QByteArray flashEntry = m_memData[getFullParams()];
    ushort reducedCount = std::min(count, ushort(flashEntry.size()));
    for(int i=0; i<reducedCount; i++)
        data[i] = flashEntry[i];
    m_memDataReadCounts[getFullParams()]++;
    return count;
}

int MockEepromDevice::Reset()
{
    if (m_globalError)
        return 0;
    doReset(0);
    return getByteSize();
}

int MockEepromDevice::getByteSize() const
{
    return m_byteCapacity;
}

void MockEepromDevice::cleanAll()
{
    m_memData.clear();
    m_memDataReadCounts.clear();
    m_memDataWriteCounts.clear();
    m_globalError = false;
}

void MockEepromDevice::setGlobalError(bool error)
{
    m_globalError = error;
}

QByteArray MockEepromDevice::getData(const I2cAddressParameter &i2cAddressParam,
                                     const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    EepromWithMuxParams params{i2cAddressParam, i2cAddressMux, muxChannelNo};
    QByteArray ret;
    if(m_memData.contains(params))
        ret = m_memData[params];
    return ret;
}

void MockEepromDevice::setData(const I2cAddressParameter &i2cAddressParam, const QByteArray &data,
                               const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    EepromWithMuxParams params{i2cAddressParam, i2cAddressMux, muxChannelNo};
    m_memData[params] = data;
}

int MockEepromDevice::getReadCount(const I2cAddressParameter &i2cAddressParam,
                                   const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    EepromWithMuxParams params{i2cAddressParam, i2cAddressMux, muxChannelNo};
    return m_memDataReadCounts[params];
}

int MockEepromDevice::getWriteCount(const I2cAddressParameter &i2cAddressParam,
                                    const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    EepromWithMuxParams params{i2cAddressParam, i2cAddressMux, muxChannelNo};
    return m_memDataWriteCounts[params];
}

void MockEepromDevice::doReset(int size)
{
    m_memData[getFullParams()] = QByteArray(size, 0xff);
}

EepromWithMuxParams MockEepromDevice::getFullParams() const
{
    return {m_i2cAddress, m_i2cAddressMux, m_muxChannelNo};
}
