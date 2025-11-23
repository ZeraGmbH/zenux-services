#ifndef MOCKEEPROM24LC_H
#define MOCKEEPROM24LC_H

#include <eepromi2cdeviceinterface.h>
#include <i2caddressparameter.h>
#include <i2cmuxerinterface.h>
#include <QHash>

class MockEEprom24LC : public EepromI2cDeviceInterface
{
public:
    static constexpr qint8 InvalidMux = qint8(I2cMuxerInterface::InvalidMux);
    MockEEprom24LC(const I2cAddressParameter &i2cAddressParam, int byteCapacity,
                   const I2cAddressParameter &i2cAddressMux = {"", I2cMuxerInterface::InvalidMux},
                   qint8 muxChannelNo = InvalidMux);
    int WriteData(char* data, ushort count, ushort adr) override;
    int ReadData(char* data, ushort count, ushort adr) override;
    int Reset() override;
    int getByteSize() const override;

    void returnReduceCountOnErrorRead();
    static void mockCleanAll();
    static QByteArray mockGetData(const I2cAddressParameter &i2cAddressParam);
    static void mockSetData(const I2cAddressParameter &i2cAddressParam, QByteArray data);
    static int mockGetReadCount(const I2cAddressParameter &i2cAddressParam);
    static int mockGetWriteCount(const I2cAddressParameter &i2cAddressParam);
    static bool mockWriteToFile(const I2cAddressParameter &i2cAddressParam, QString fileName);
    static bool mockReadFromFile(const I2cAddressParameter &i2cAddressParam, QString fileName);
    static bool mockCompareWithFile(const I2cAddressParameter &i2cAddressParam, QString fileName);
private:
    void doReset(int size);

    struct I2cAddressParameter m_i2cAddress;
    const int m_byteCapacity;
    struct I2cAddressParameter m_i2cAddressMux;
    const qint8 m_muxChannelNo;

    static QHash<I2cAddressParameter, QByteArray> m_flashData;
    static QHash<I2cAddressParameter, int> m_flashDataReadCounts;
    static QHash<I2cAddressParameter, int> m_flashDataWriteCounts;
    static QHash<I2cAddressParameter, bool> m_returnReducedDataSizeOnRead;
};

inline bool operator==(const I2cAddressParameter& a1,
                       const I2cAddressParameter& a2) {
    return a1.i2cAddr == a2.i2cAddr &&
           a1.devNodeFileName == a2.devNodeFileName;
}
inline uint qHash(const I2cAddressParameter& key, uint seed = 0) {
    const QString hashable = key.devNodeFileName +
                             QString::number(key.i2cAddr);
    return qHash(hashable, seed);
}

#endif // MOCKEEPROM24LC_H
