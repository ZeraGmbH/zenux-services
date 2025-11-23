#ifndef MOCKEEPROMI2CFACTORY_H
#define MOCKEEPROMI2CFACTORY_H

#include <abstracteepromi2cfactory.h>
#include <QHash>

struct EepromWithMuxParams {
    I2cAddressParameter m_i2cAddress;
    I2cAddressParameter m_i2cAddressMux;
    qint8 m_muxChannelNo;
}; // move?

class MockEepromI2cFactory : public AbstractEepromI2cFactory
{
public:
    EepromI2cDeviceInterfacePtr createEeprom(const I2cAddressParameter &i2cAddress, int byteCapacity) override;
    EepromI2cDeviceInterfacePtr createEepromOnMux(const I2cAddressParameter &i2cAddress, int byteCapacity,
                                                  const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo) override;
private:
    friend class MockEEprom24LC;
    static QHash<EepromWithMuxParams, QByteArray> m_flashData;
    static QHash<EepromWithMuxParams, int> m_flashDataReadCounts;
    static QHash<EepromWithMuxParams, int> m_flashDataWriteCounts;
    static QHash<EepromWithMuxParams, bool> m_returnReducedDataSizeOnRead;
};


inline bool operator==(const EepromWithMuxParams& a1,
                       const EepromWithMuxParams& a2) {
    return a1.m_i2cAddress.devNodeFileName    == a2.m_i2cAddress.devNodeFileName &&
           a1.m_i2cAddress.i2cAddr            == a2.m_i2cAddress.i2cAddr &&
           a1.m_i2cAddressMux.devNodeFileName == a2.m_i2cAddressMux.devNodeFileName &&
           a1.m_i2cAddressMux.i2cAddr         == a2.m_i2cAddressMux.i2cAddr &&
           a1.m_muxChannelNo                  == a2.m_muxChannelNo;
}
inline uint qHash(const EepromWithMuxParams& key, uint seed = 0) {
    const QString hashable = key.m_i2cAddress.devNodeFileName +
                             QString::number(key.m_i2cAddress.i2cAddr) +
                             key.m_i2cAddressMux.devNodeFileName +
                             QString::number(key.m_i2cAddressMux.i2cAddr) +
                             QString::number(key.m_muxChannelNo);
    return qHash(hashable, seed);
}

#endif // MOCKEEPROMI2CFACTORY_H
