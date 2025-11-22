#include "mockeeprom24lc.h"
#include <QFile>

QHash<EepromI2cDeviceInterface::AddressData, QByteArray> MockEEprom24LC::m_flashData;
QHash<EepromI2cDeviceInterface::AddressData, int>        MockEEprom24LC::m_flashDataReadCounts;
QHash<EepromI2cDeviceInterface::AddressData, int>        MockEEprom24LC::m_flashDataWriteCounts;
QHash<EepromI2cDeviceInterface::AddressData, bool>       MockEEprom24LC::m_returnReducedDataSizeOnRead;

MockEEprom24LC::MockEEprom24LC(const EepromI2cDeviceInterface::AddressData &addressData, int byteCapacity) :
    EepromI2cDeviceInterface(byteCapacity),
    m_addressInfo(addressData)
{
    if(m_flashData.contains(addressData))
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
    QByteArray &flashEntry = m_flashData[m_addressInfo];
    for(int i=0; i<count; i++)
        flashEntry[i] = data[i];
    m_flashDataWriteCounts[m_addressInfo]++;
    return count;
}

int MockEEprom24LC::ReadData(char *data, ushort count, ushort adr)
{
    if(adr != 0)
        qFatal("Address other than 0 is not yet supported!");
    if(count > getByteSize())
        qFatal("Cannot read data of length %i / max is %i", count, getByteSize());

    const QByteArray flashEntry = m_flashData[m_addressInfo];
    ushort reducedCount = std::min(count, ushort(flashEntry.size()));
    for(int i=0; i<reducedCount; i++)
        data[i] = flashEntry[i];
    m_flashDataReadCounts[m_addressInfo]++;
    bool reduceCount = m_returnReducedDataSizeOnRead[m_addressInfo];
    return reduceCount ? reducedCount: count;
}

int MockEEprom24LC::Reset()
{
    doReset(0);
    return getByteSize();
}

void MockEEprom24LC::returnReduceCountOnErrorRead()
{
    m_returnReducedDataSizeOnRead[m_addressInfo] = true;
}

void MockEEprom24LC::mockCleanAll()
{
    m_flashData.clear();
    m_flashDataReadCounts.clear();
    m_flashDataWriteCounts.clear();
    m_returnReducedDataSizeOnRead.clear();
}

QByteArray MockEEprom24LC::mockGetData(const EepromI2cDeviceInterface::AddressData &addressData)
{
    QByteArray ret;
    if(m_flashData.contains(addressData))
        ret = m_flashData[addressData];
    return ret;
}

void MockEEprom24LC::mockSetData(const EepromI2cDeviceInterface::AddressData &addressData, QByteArray data)
{
    m_flashData[addressData] = data;
}

int MockEEprom24LC::mockGetReadCount(const EepromI2cDeviceInterface::AddressData &addressData)
{
    return m_flashDataReadCounts[addressData];
}

int MockEEprom24LC::mockGetWriteCount(const EepromI2cDeviceInterface::AddressData &addressData)
{
    return m_flashDataWriteCounts[addressData];
}

bool MockEEprom24LC::mockWriteToFile(const EepromI2cDeviceInterface::AddressData &addressData, QString fileName)
{
    QByteArray data = mockGetData(addressData);
    if(data.isEmpty())
        return false;
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly))
        return file.write(data) == data.length();
    return false;
}

bool MockEEprom24LC::mockReadFromFile(const EepromI2cDeviceInterface::AddressData &addressData, QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        if(data.isEmpty())
            return false;
        mockSetData(addressData, data);
        return true;
    }
    return false;
}

bool MockEEprom24LC::mockCompareWithFile(const EepromI2cDeviceInterface::AddressData &addressData, QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        if(data.isEmpty()) // for now...
            return false;
        return data == mockGetData(addressData);
    }
    return false;
}

void MockEEprom24LC::doReset(int size)
{
    m_flashData[m_addressInfo] = QByteArray(size, 0xff);
}
