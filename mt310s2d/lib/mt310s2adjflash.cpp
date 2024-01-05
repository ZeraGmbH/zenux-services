#include "mt310s2adjflash.h"
#include <QBuffer>
#include <F24LC256.h>
#include <i2cmuxerscopedonoff.h>
#include "i2ceepromiofactory.h"

Mt310s2AdjFlash::Mt310s2AdjFlash(QString devnode, quint8 i2cadr, I2cMuxerInterface::Ptr i2cMuxer) :
    m_sDeviceNode(devnode),
    m_nI2CAdr(i2cadr),
    m_i2cMuxer(i2cMuxer)
{
}

bool Mt310s2AdjFlash::exportAdjFlash(QDateTime dateTimeWrite)
{
    bool ret;
    QByteArray ba;
    QDataStream stream(&ba,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_4);

    quint32 count = 0;
    quint16 chksum = 0;

    stream << count; // first we write place holders for count and chksum this is the same for each adjflash object
    stream << chksum;

    exportAdjData(stream, dateTimeWrite);
    setAdjCountChecksum(ba);
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    ret = writeFlash(ba);
    return ret;
}


bool Mt310s2AdjFlash::importAdjFlash()
{
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    QByteArray ba;
    if (readFlash(ba)) { // if we could read data with correct chksum
        QDataStream stream(&ba, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_4);
        return importAdjData(stream);
    }
    return false;
}

bool Mt310s2AdjFlash::resetAdjFlash()
{
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    I2cFlashInterfacePtrU flashIo = I2cEEpromIoFactory::create24LC256(m_sDeviceNode, m_nI2CAdr);
    return flashIo->Reset() == flashIo->size();
}


void Mt310s2AdjFlash::setAdjCountChecksum(QByteArray &ba)
{
    quint32 count;

    m_nChecksum = 0;
    count = ba.size();

    QByteArray ca(6, 0); // qbyte array mit 6 bytes
    QDataStream castream( &ca, QIODevice::WriteOnly );
    castream.setVersion(QDataStream::Qt_5_4);

    castream << count << m_nChecksum;

    QBuffer mem(&ba);
    mem.open(QIODevice::ReadWrite);
    mem.seek(0); // positioning qbuffer to chksum
    mem.write(ca); // we set count here  and chksum to 0

    m_nChecksum = qChecksum(ba.data(),ba.size()); // +crc-16

    QDataStream ca2stream( &ca, QIODevice::WriteOnly );
    ca2stream.setVersion(QDataStream::Qt_5_4);

    ca2stream << count << m_nChecksum;

    mem.seek(0); // positioning qbuffer to chksum
    mem.write(ca); // setting correct chksum now
    mem.close(); // wird nicht mehr benötigt
}


bool Mt310s2AdjFlash::writeFlash(QByteArray &ba)
{
    int count = ba.size();
    I2cFlashInterfacePtrU flashIo = I2cEEpromIoFactory::create24LC256(m_sDeviceNode, m_nI2CAdr);
    int written = flashIo->WriteData(ba.data(), count, 0);
    if ( (count - written) > 0) {
        qCritical("Error on flash memory write: wanted: %i / written: %i", count, written);
        return false; // fehler beim flash schreiben
    }
    return true;
}

quint16 Mt310s2AdjFlash::getChecksum()
{
    return m_nChecksum;
}

I2cMuxerInterface::Ptr Mt310s2AdjFlash::getI2cMuxer()
{
    return m_i2cMuxer;
}

bool Mt310s2AdjFlash::readFlash(QByteArray &ba)
{
    // first we try to read 6 bytes hold length (quint32) and checksum (quint16)
    const int headerLen = 6;
    ba.resize(headerLen);
    I2cFlashInterfacePtrU flashIo = I2cEEpromIoFactory::create24LC256(m_sDeviceNode, m_nI2CAdr);
    int bytesRead = flashIo->ReadData(ba.data(), headerLen, 0);
    if ( bytesRead != headerLen ) {
        qCritical("Error on flash read: expected: %i / read %i", headerLen, bytesRead);
        return(false); // read error
    }

    QDataStream bastream( &ba, QIODevice::ReadOnly );
    bastream.setVersion(QDataStream::Qt_5_4);

    quint32 count;

    bastream >> count >> m_nChecksum;
    if ( count > (quint32)flashIo->size() ) {
        qCritical("Cannot read flash: size wanted: %i / available %i", count, flashIo->size());
        return(false); // read error
    }

    ba.resize(count);

    if ( (count - flashIo->ReadData(ba.data(), count, 0)) > 0 ) {
        qCritical("Error on flash read: wanted: %i / available %i", count, flashIo->size());
        return(false); // read error
    }

    QBuffer mem;
    mem.setBuffer(&ba);
    mem.open(QIODevice::ReadWrite);

    mem.seek(4);

    QByteArray ca(2, 0); // qbyte array mit 6 bytes
    mem.write(ca); // 0 setzen der checksumme
    mem.close();

    quint16 chksum;
    chksum = qChecksum(ba.data(),ba.size()); // +crc-16

    return (chksum == m_nChecksum); // we could read count bytes and the chksum is ok.
}
