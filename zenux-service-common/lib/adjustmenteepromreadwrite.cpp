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
    qInfo("Read adjustment data...");
    if(m_adjDataReadIsValid) {
        qInfo("Adjustment data was not changed since last valid read. No read necessary.");
        return true;
    }
    m_adjDataReadIsValid = false;
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    I2cFlashInterfacePtrU memIo = I2cEEpromIoFactory::create24LC256(m_sDeviceNode, m_nI2CAdr);
    QByteArray ba;
    if(readSizeAndChecksum(memIo.get()) && readEepromChecksumValidated(memIo.get(), ba)) {
        qInfo("Read adjustment data passed.");
        m_adjData = ba;
        m_adjDataReadIsValid = true;
    }
    else
        qWarning("Read adjustment data failed!");
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

bool AdjustmentEepromReadWrite::readSizeAndChecksum(I2cFlashInterface *memInterface)
{
    const int headerLen = sizeof(m_adjSize) + sizeof(m_nChecksum);
    QByteArray ba;
    ba.resize(headerLen);
    int bytesRead = memInterface->ReadData(ba.data(), headerLen, 0);
    if(bytesRead != headerLen) {
        qCritical("Error on flash read: expected: %i / read %i", headerLen, bytesRead);
        return false;
    }
    QDataStream bastream(&ba, QIODevice::ReadOnly);
    bastream.setVersion(QDataStream::Qt_5_4);
    bastream >> m_adjSize >> m_nChecksum;
    quint32 memSize = memInterface->size();
    if(m_adjSize > memSize) {
        qWarning("EEPROM size wanted: %u is larger than available %u - fresh EEPROM?", m_adjSize, memSize);
        return false;
    }
    return true;
}

bool AdjustmentEepromReadWrite::readEepromChecksumValidated(I2cFlashInterface *memInterface, QByteArray &ba)
{
    ba.resize(m_adjSize);
    quint32 sizeRead = memInterface->ReadData(ba.data(), m_adjSize, 0);
    if (sizeRead < m_adjSize) {
        qCritical("Error on flash read: wanted: %i / available %i / got %i",
                  m_adjSize, memInterface->size(), sizeRead);
        return false;
    }
    // for checksum calculation checksum in buffer is set 0
    // TODO?: solve different states
    // * after read: checksum = 0
    // * after write: checksum as calculated...
    setChecksumInBuffer(ba, 0);
    quint16 chksum = qChecksum(ba.data(), ba.size()); // +crc-16
    return (chksum == m_nChecksum);
}

bool AdjustmentEepromReadWrite::writeFlash(QByteArray &ba)
{
    int count = ba.size();
    I2cFlashInterfacePtrU flashIo = I2cEEpromIoFactory::create24LC256(m_sDeviceNode, m_nI2CAdr);
    int written = flashIo->WriteData(ba.data(), count, 0);
    if ( (count - written) > 0) {
        qCritical("Error on flash memory write: wanted: %i / written: %i", count, written);
        return false;
    }
    return true;
}

void AdjustmentEepromReadWrite::setAdjCountChecksum(QByteArray &ba)
{
    m_nChecksum = 0;
    quint32 count = ba.size();

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

void AdjustmentEepromReadWrite::setChecksumInBuffer(QByteArray &ba, quint16 checksum)
{
    QDataStream stream(&ba, QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_4);
    stream.skipRawData(sizeof(m_adjSize));
    stream << checksum;
}
