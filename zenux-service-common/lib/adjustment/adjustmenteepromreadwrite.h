#ifndef ADJUSTMENTEEPROMREADWRITE_H
#define ADJUSTMENTEEPROMREADWRITE_H

#include <abstracteepromi2cdevice.h>
#include <i2caddressparameter.h>
#include <i2cmuxerinterface.h>
#include <QString>

class AdjustmentEepromReadWrite
{
public:
    AdjustmentEepromReadWrite(EepromI2cDeviceInterfacePtr adjMemory);
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
    bool readSizeAndChecksum(quint32 &sizeRead);
    bool readAllAndValidateFromChip(QByteArray& ba, quint32 size);
    bool readAllAndValidateFromCache(QByteArray& ba, quint32 size, QString cacheFileName);
    bool writeRawDataToChip(QByteArray& ba);
    void writeRawDataToCache(QByteArray ba, QString cacheFileName);
    void setCountAndChecksum(QByteArray& ba);
    void setChecksumInBuffer(QByteArray& ba, quint16 checksum);
    void setCountInBuffer(QByteArray& ba);

    EepromI2cDeviceInterfacePtr m_adjMemory;
    quint16 m_checksum = 0;
    QByteArray m_adjData;
    bool m_adjDataReadIsValid = false;
    static QString m_cachePath;
};

#endif // ADJUSTMENTEEPROMREADWRITE_H
