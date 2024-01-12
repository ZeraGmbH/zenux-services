#ifndef ADJUSTMENTEEPROM_H
#define ADJUSTMENTEEPROM_H

#include "adjustmentstatusinterface.h"
#include <i2cmuxerinterface.h>
#include <QByteArray>
#include <QString>
#include <QDataStream>
#include <QDateTime>

class AdjustmentEeprom
{
public:
    AdjustmentEeprom(){}
    AdjustmentEeprom(QString devnode, quint8 i2cadr, I2cMuxerInterface::Ptr i2cMuxer);
    bool exportAdjFlash(QDateTime dateTimeWrite);
    bool importAdjFlash();
    bool resetAdjFlash();

    quint16 getChecksum();
    I2cMuxerInterface::Ptr getI2cMuxer();
protected:
    virtual void exportAdjData(QDataStream& stream, QDateTime dateTimeWrite) = 0; // the derived class exports adjdata to qdatastream
    virtual bool importAdjData(QDataStream& stream) = 0; // same for import
    bool readEepromChecksumValidated(QByteArray& ba);

private:
    void setAdjCountChecksum(QByteArray& ba);
    bool writeFlash(QByteArray& ba);

    quint16 m_nChecksum = 0;
    QString m_sDeviceNode;
    quint8 m_nI2CAdr;
    I2cMuxerInterface::Ptr m_i2cMuxer;
};

#endif // ADJUSTMENTEEPROM_H
