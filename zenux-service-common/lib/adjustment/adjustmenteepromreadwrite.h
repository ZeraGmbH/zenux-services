#ifndef ADJUSTMENTEEPROMREADWRITE_H
#define ADJUSTMENTEEPROMREADWRITE_H

#include <eepromi2cdeviceinterface.h>
#include <i2cmuxerinterface.h>
#include <QString>

class AdjustmentEepromReadWrite
{
public:
    AdjustmentEepromReadWrite(QString devnode, quint8 i2cadr, int byteCapacity,
                              I2cMuxerInterface::Ptr i2cMuxer);
    static void setCachePath(QString path);
    static QString getCacheFullFileName(QString cacheFileName);
    bool readData();
    bool readDataCached(QString cacheFileName);
    bool writeData();
    bool resetData();

    quint32 getMaxSize();
    quint16 getChecksum();
    QByteArray getData();
    void setData(const QByteArray& ba);

private:
    bool readSizeAndChecksum(EepromI2cDeviceInterface *memInterface, quint32 &sizeRead);
    bool readAllAndValidateFromChip(EepromI2cDeviceInterface *memInterface, QByteArray& ba, quint32 size);
    bool readAllAndValidateFromCache(QByteArray& ba, quint32 size, QString cacheFileName);
    bool writeRawDataToChip(QByteArray& ba);
    void writeRawDataToCache(QByteArray ba, QString cacheFileName);
    void setCountAndChecksum(QByteArray& ba);
    void setChecksumInBuffer(QByteArray& ba, quint16 checksum);
    void setCountInBuffer(QByteArray& ba);

    quint16 m_checksum = 0;
    QString m_sDeviceNode;
    quint8 m_i2cAdr;
    const int m_byteCapacity;
    I2cMuxerInterface::Ptr m_i2cMuxer;
    QByteArray m_adjData;
    bool m_adjDataReadIsValid = false;
    static QString m_cachePath;
};

#endif // ADJUSTMENTEEPROMREADWRITE_H
