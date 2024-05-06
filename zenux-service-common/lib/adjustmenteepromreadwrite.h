#ifndef ADJUSTMENTEEPROMREADWRITE_H
#define ADJUSTMENTEEPROMREADWRITE_H

#include <i2cmuxerinterface.h>
#include <QString>

class AdjustmentEepromReadWrite
{
public:
    AdjustmentEepromReadWrite(QString devnode, quint8 i2cadr, I2cMuxerInterface::Ptr i2cMuxer);
    bool importAdjFlash();
    bool exportAdjFlash();
    bool resetAdjFlash();

    QByteArray getAdjData();
    void setAdjData(const QByteArray& ba);

    I2cMuxerInterface::Ptr getI2cMuxer();
    quint16 getChecksum();

private:
    bool readEepromChecksumValidated(QByteArray& ba);
    bool writeFlash(QByteArray& ba);
    void setAdjCountChecksum(QByteArray& ba);

    quint16 m_nChecksum = 0;
    QString m_sDeviceNode;
    quint8 m_nI2CAdr;
    I2cMuxerInterface::Ptr m_i2cMuxer;
    QByteArray m_adjData;
};

#endif // ADJUSTMENTEEPROMREADWRITE_H
