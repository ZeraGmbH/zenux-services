#include "mockeepromdevice.h"
#include <QFile>

QHash<EepromWithMuxParams, QByteArray> MockEepromDevice::m_flashData;
QHash<EepromWithMuxParams, int>        MockEepromDevice::m_flashDataReadCounts;
QHash<EepromWithMuxParams, int>        MockEepromDevice::m_flashDataWriteCounts;
bool                                   MockEepromDevice::m_globalError = false;

MockEepromDevice::MockEepromDevice(const I2cAddressParameter &i2cAddressParam, int byteCapacity,
                                   const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo) :
    m_i2cAddress(i2cAddressParam),
    m_byteCapacity(byteCapacity),
    m_i2cAddressMux(i2cAddressMux),
    m_muxChannelNo(muxChannelNo)
{
    EepromWithMuxParams fullParams = getFullParams();
    if(m_flashData.contains(fullParams))
        return;
    // Internal memory is assumed plugged and can be 'unplugged' by setGlobalError
    if(m_muxChannelNo == InvalidMux)
        doReset(byteCapacity);
}

bool MockEepromDevice::isMemoryPlugged() const
{
    if (m_globalError)
        return false;
    return m_flashData.contains(getFullParams());
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
    QByteArray &flashEntry = m_flashData[getFullParams()];
    for(int i=0; i<count; i++)
        flashEntry[i] = data[i];
    m_flashDataWriteCounts[getFullParams()]++;
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

    const QByteArray flashEntry = m_flashData[getFullParams()];
    ushort reducedCount = std::min(count, ushort(flashEntry.size()));
    for(int i=0; i<reducedCount; i++)
        data[i] = flashEntry[i];
    m_flashDataReadCounts[getFullParams()]++;
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
    m_flashData.clear();
    m_flashDataReadCounts.clear();
    m_flashDataWriteCounts.clear();
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
    if(m_flashData.contains(params))
        ret = m_flashData[params];
    return ret;
}

void MockEepromDevice::setData(const I2cAddressParameter &i2cAddressParam, QByteArray data,
                               const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    EepromWithMuxParams params{i2cAddressParam, i2cAddressMux, muxChannelNo};
    m_flashData[params] = data;
}

int MockEepromDevice::getReadCount(const I2cAddressParameter &i2cAddressParam,
                                   const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    EepromWithMuxParams params{i2cAddressParam, i2cAddressMux, muxChannelNo};
    return m_flashDataReadCounts[params];
}

int MockEepromDevice::getWriteCount(const I2cAddressParameter &i2cAddressParam,
                                    const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    EepromWithMuxParams params{i2cAddressParam, i2cAddressMux, muxChannelNo};
    return m_flashDataWriteCounts[params];
}

void MockEepromDevice::doReset(int size)
{
    m_flashData[getFullParams()] = QByteArray(size, 0xff);
}

EepromWithMuxParams MockEepromDevice::getFullParams() const
{
    return {m_i2cAddress, m_i2cAddressMux, m_muxChannelNo};
}
