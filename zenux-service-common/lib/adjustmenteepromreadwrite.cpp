#include "adjustmenteepromreadwrite.h"
#include "i2ceepromiofactory.h"
#include <i2cmuxerscopedonoff.h>
#include <QDataStream>
#include <QBuffer>

AdjustmentEepromReadWrite::AdjustmentEepromReadWrite(QString devnode, quint8 i2cadr, I2cMuxerInterface::Ptr i2cMuxer) :
    m_sDeviceNode(devnode),
    m_nI2CAdr(i2cadr),
    m_i2cMuxer(i2cMuxer)
{
}

bool AdjustmentEepromReadWrite::importAdjFlash()
{
    if(m_adjDataReadIsValid) {
        qInfo("Adjustment data was not changed since last valid read. Do consider still valid,");
        return true;
    }
    qInfo("Import AdjFlash...");
    m_adjDataReadIsValid = false;
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    QByteArray ba;
    if (readEepromChecksumValidated(ba)) {
        m_adjData = ba;
        qInfo("Import AdjFlash passed.");
        m_adjDataReadIsValid = true;
    }
    qWarning("Import AdjFlash failed");
    return m_adjDataReadIsValid;
}

bool AdjustmentEepromReadWrite::exportAdjFlash()
{
    // There are different places not properly designed
    // importAdjFlash is bound to decode so to make writes take effect
    // reading is mandatory. So force a read
    m_adjDataReadIsValid = false;
    setAdjCountChecksum(m_adjData);
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    return writeFlash(m_adjData);
}

bool AdjustmentEepromReadWrite::resetAdjFlash()
{
    m_adjDataReadIsValid = false;
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    I2cFlashInterfacePtrU flashIo = I2cEEpromIoFactory::create24LC256(m_sDeviceNode, m_nI2CAdr);
    return flashIo->Reset() == flashIo->size();
}

// Current (valid) assumption: All devices have 24LC256 with 32kBytes
quint32 AdjustmentEepromReadWrite::getMaxSize()
{
    I2cFlashInterfacePtrU flashIo = I2cEEpromIoFactory::create24LC256(m_sDeviceNode, m_nI2CAdr);
    return flashIo->size();
}

QByteArray AdjustmentEepromReadWrite::getAdjData()
{
    return m_adjData;
}

void AdjustmentEepromReadWrite::setAdjData(const QByteArray &ba)
{
    m_adjData = ba;
}

I2cMuxerInterface::Ptr AdjustmentEepromReadWrite::getI2cMuxer()
{
    return m_i2cMuxer;
}

quint16 AdjustmentEepromReadWrite::getChecksum()
{
    return m_nChecksum;
}

bool AdjustmentEepromReadWrite::readEepromChecksumValidated(QByteArray &ba)
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

    QDataStream bastream(&ba, QIODevice::ReadOnly );
    bastream.setVersion(QDataStream::Qt_5_4);

    quint32 count;
    bastream >> count >> m_nChecksum;
    if ( count > (quint32)flashIo->size() ) {
        qWarning("EEPROM size wanted: %u is larger than available %i - fresh EEPROM?", count, flashIo->size());
        return(false); // read error
    }

    ba.resize(count);
    quint32 sizeRead = flashIo->ReadData(ba.data(), count, 0);
    if (sizeRead < count) {
        qCritical("Error on flash read: wanted: %i / available %i / got %i",
                  count, flashIo->size(), sizeRead);
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

bool AdjustmentEepromReadWrite::writeFlash(QByteArray &ba)
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

void AdjustmentEepromReadWrite::setAdjCountChecksum(QByteArray &ba)
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
