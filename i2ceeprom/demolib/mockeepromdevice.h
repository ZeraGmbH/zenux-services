#ifndef MOCKEEPROMDEVICE_H
#define MOCKEEPROMDEVICE_H

#include "mockeepromi2cfactory.h"
#include <abstracteepromi2cdevice.h>
#include <i2caddressparameter.h>
#include <i2cmuxerinterface.h>
#include <QHash>

class MockEepromDevice : public AbstractEepromI2cDevice
{
public:
    static constexpr qint8 InvalidMux = qint8(I2cMuxerInterface::InvalidMux);
    MockEepromDevice(const I2cAddressParameter &i2cAddressParam, int byteCapacity,
                   const I2cAddressParameter &i2cAddressMux = {"", I2cMuxerInterface::InvalidMux},
                   qint8 muxChannelNo = InvalidMux);
    bool isMemoryPlugged() const override;
    int WriteData(char* data, ushort count, ushort adr) override;
    int ReadData(char* data, ushort count, ushort adr) override;
    int Reset() override;
    int getByteSize() const override;

    static void cleanAll();
    static void setGlobalError(bool error);
    static QByteArray getData(const I2cAddressParameter &i2cAddressParam,
                              const I2cAddressParameter &i2cAddressMux = {"", I2cMuxerInterface::InvalidMux},
                              qint8 muxChannelNo = InvalidMux);
    static void setData(const I2cAddressParameter &i2cAddressParam, QByteArray data,
                        const I2cAddressParameter &i2cAddressMux = {"", I2cMuxerInterface::InvalidMux},
                        qint8 muxChannelNo = InvalidMux);
    static int getReadCount(const I2cAddressParameter &i2cAddressParam,
                            const I2cAddressParameter &i2cAddressMux = {"", I2cMuxerInterface::InvalidMux},
                            qint8 muxChannelNo = InvalidMux);
    static int getWriteCount(const I2cAddressParameter &i2cAddressParam,
                             const I2cAddressParameter &i2cAddressMux = {"", I2cMuxerInterface::InvalidMux},
                             qint8 muxChannelNo = InvalidMux);
private:
    void doReset(int size);
    struct EepromWithMuxParams getFullParams() const;

    struct I2cAddressParameter m_i2cAddress;
    const int m_byteCapacity;
    struct I2cAddressParameter m_i2cAddressMux;
    const qint8 m_muxChannelNo;

    static QHash<EepromWithMuxParams, QByteArray> m_flashData;
    static QHash<EepromWithMuxParams, int>        m_flashDataReadCounts;
    static QHash<EepromWithMuxParams, int>        m_flashDataWriteCounts;
    static bool                                   m_globalError;
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

#endif // MOCKEEPROMDEVICE_H
