#ifndef ADJUSTMENTEEPROMREADWRITE_H
#define ADJUSTMENTEEPROMREADWRITE_H

#include <i2cflashinterface.h>
#include <i2cmuxerinterface.h>
#include <QString>

class AdjustmentEepromReadWrite
{
public:
    AdjustmentEepromReadWrite(QString devnode, quint8 i2cadr, I2cMuxerInterface::Ptr i2cMuxer);
    bool readData();
    bool writeData();
    bool resetData();

    quint32 getMaxSize();
    quint16 getChecksum();
    QByteArray getData();
    void setData(const QByteArray& ba);

private:
    bool readSizeAndChecksum(I2cFlashInterface *memInterface, quint32 &sizeRead);
    bool readAllAndValidate(I2cFlashInterface *memInterface, QByteArray& ba, quint32 size);
    bool writeRawDataToChip(QByteArray& ba);
    void writeRawDataToCache(QByteArray& ba);
    void setCountAndChecksum(QByteArray& ba);
    void setChecksumInBuffer(QByteArray& ba, quint16 checksum);
    void setCountInBuffer(QByteArray& ba);

    quint16 m_checksum = 0;
    QString m_sDeviceNode;
    quint8 m_i2cAdr;
    I2cMuxerInterface::Ptr m_i2cMuxer;
    QByteArray m_adjData;
    bool m_adjDataReadIsValid = false;
};

#endif // ADJUSTMENTEEPROMREADWRITE_H
