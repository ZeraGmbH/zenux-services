#ifndef MOCKEEPROM24LC_H
#define MOCKEEPROM24LC_H

#include <eepromi2cdeviceinterface.h>
#include <QHash>

class MockEEprom24LC : public EepromI2cDeviceInterface
{
public:
    MockEEprom24LC(const EepromI2cDeviceInterface::AddressData &addressData, int byteCapacity);
    int WriteData(char* data, ushort count, ushort adr) override;
    int ReadData(char* data, ushort count, ushort adr) override;
    virtual int Reset() override;

    void returnReduceCountOnErrorRead();
    static void mockCleanAll();
    static QByteArray mockGetData(const EepromI2cDeviceInterface::AddressData &addressData);
    static void mockSetData(const EepromI2cDeviceInterface::AddressData &addressData, QByteArray data);
    static int mockGetReadCount(const EepromI2cDeviceInterface::AddressData &addressData);
    static int mockGetWriteCount(const EepromI2cDeviceInterface::AddressData &addressData);
    static bool mockWriteToFile(const EepromI2cDeviceInterface::AddressData &addressData, QString fileName);
    static bool mockReadFromFile(const EepromI2cDeviceInterface::AddressData &addressData, QString fileName);
    static bool mockCompareWithFile(const EepromI2cDeviceInterface::AddressData &addressData, QString fileName);
private:
    void doReset(int size);

    struct EepromI2cDeviceInterface::AddressData m_addressInfo;

    static QHash<EepromI2cDeviceInterface::AddressData, QByteArray> m_flashData;
    static QHash<EepromI2cDeviceInterface::AddressData, int> m_flashDataReadCounts;
    static QHash<EepromI2cDeviceInterface::AddressData, int> m_flashDataWriteCounts;
    static QHash<EepromI2cDeviceInterface::AddressData, bool> m_returnReducedDataSizeOnRead;
};

inline bool operator==(const EepromI2cDeviceInterface::AddressData& a1,
                       const EepromI2cDeviceInterface::AddressData& a2) {
    return a1.i2cAddr == a2.i2cAddr &&
           a1.channel0to7 == a2.channel0to7 &&
           a1.i2cMuxAddr == a2.i2cMuxAddr &&
           a1.devNodeFileName == a2.devNodeFileName;
}
inline uint qHash(const EepromI2cDeviceInterface::AddressData& key, uint seed = 0) {
    const QString hashable = key.devNodeFileName +
                             QString::number(key.i2cAddr) +
                             QString::number(key.i2cMuxAddr) +
                             QString::number(key.channel0to7);
    return qHash(hashable, seed);
}

#endif // MOCKEEPROM24LC_H
