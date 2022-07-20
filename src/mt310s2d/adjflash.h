#ifndef ADJFLASH_H
#define ADJFLASH_H

#include "i2cmuxer.h"
#include <QByteArray>
#include <QString>
#include <QDataStream>

class cAdjFlash
{
public:
    cAdjFlash(){}
    cAdjFlash(QString devnode, quint8 i2cadr, I2cMuxerInterface::Ptr i2cMuxer);
    bool exportAdjFlash();
    bool importAdjFlash();
    bool resetAdjFlash();

    virtual quint8 getAdjustmentStatus() = 0;
    quint16 getChecksum();
protected:
    virtual void exportAdjData(QDataStream& stream) = 0; // the derived class exports adjdata to qdatastream
    virtual bool importAdjData(QDataStream& stream) = 0; // same for import
    bool readFlash(QByteArray& ba);
    bool writeFlash(QByteArray& ba);
    QString m_sDeviceNode;
    quint8 m_nI2CAdr;
    quint16 m_nChecksum;
    I2cMuxerInterface::Ptr m_i2cMuxer;
private:
    void setAdjCountChecksum(QByteArray& ba);
};

#endif // ADJFLASH_H
