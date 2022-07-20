#include "adjflash.h"
#include "mt310s2dglobal.h"
#include <QBuffer>
#include <syslog.h>
#include <F24LC256.h>
#include <i2cmuxerscopedonoff.h>

cAdjFlash::cAdjFlash(QString devnode, quint8 i2cadr, I2cMuxerInterface::Ptr i2cMuxer) :
    m_sDeviceNode(devnode),
    m_nI2CAdr(i2cadr),
    m_i2cMuxer(i2cMuxer)
{
}

bool cAdjFlash::exportAdjFlash()
{
    bool ret;
    QByteArray ba;
    QDataStream stream(&ba,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_4);

    quint32 count = 0;
    quint16 chksum = 0;

    stream << count; // first we write place holders for count and chksum this is the same for each adjflash object
    stream << chksum;

    exportAdjData(stream);
    setAdjCountChecksum(ba);
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    ret = writeFlash(ba);
    return ret;
}


bool cAdjFlash::importAdjFlash()
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

bool cAdjFlash::resetAdjFlash()
{
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    cF24LC256 flash(m_sDeviceNode, m_nI2CAdr);
    return flash.Reset() == flash.size();
}


void cAdjFlash::setAdjCountChecksum(QByteArray &ba)
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


bool cAdjFlash::writeFlash(QByteArray &ba)
{
    int count, written;

    cF24LC256 Flash(m_sDeviceNode, m_nI2CAdr);
    count = ba.size();
    written = Flash.WriteData(ba.data(),count,0);

    if ( (count - written) > 0) {
         syslog(LOG_ERR,"error writing flashmemory\n");
         return false; // fehler beim flash schreiben
    }
    return true;
}

quint16 cAdjFlash::getChecksum()
{
    return m_nChecksum;
}


bool cAdjFlash::readFlash(QByteArray &ba)
{
    cF24LC256 Flash(m_sDeviceNode, m_nI2CAdr);

    // first we try to read 6 bytes hold length (quint32) and checksum (quint16)
    const int headerLen = 6;
    ba.resize(headerLen);
    int bytesRead = Flash.ReadData(ba.data(), headerLen, 0);
    if ( bytesRead != headerLen ) {
        syslog(LOG_ERR,"error reading flashmemory\n");
        return(false); // read error
    }

    QDataStream bastream( &ba, QIODevice::ReadOnly );
    bastream.setVersion(QDataStream::Qt_5_4);

    quint32 count;

    bastream >> count >> m_nChecksum;
    if ( count > (quint32)Flash.size() ) {
        syslog(LOG_ERR,"error reading flashmemory, count > flash\n");
        return(false); // read error
    }

    ba.resize(count);

    if ( (count - Flash.ReadData(ba.data(),count,0)) >0 ) {
        syslog(LOG_ERR,"error reading flashmemory\n");
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
