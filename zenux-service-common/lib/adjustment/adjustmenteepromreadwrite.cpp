#include "adjustmenteepromreadwrite.h"
#include <i2cmuxerscopedonoff.h>
#include <QDataStream>
#include <QBuffer>
#include <QDir>

QString AdjustmentEepromReadWrite::m_cachePath = "/var/cache/zenux-services";

AdjustmentEepromReadWrite::AdjustmentEepromReadWrite(EepromI2cDeviceInterfacePtr adjMemory) :
    m_adjMemory(std::move(adjMemory))
{
}

void AdjustmentEepromReadWrite::setCachePath(const QString &path)
{
    m_cachePath = path;
}

QString AdjustmentEepromReadWrite::getCacheFullFileName(const QString &cacheFileName)
{
    QFileInfo fi(m_cachePath + "/" + cacheFileName);
    QString fullPath = fi.absoluteFilePath();
    return fullPath;
}

bool AdjustmentEepromReadWrite::readData()
{
    return readDataCached("");
}

bool AdjustmentEepromReadWrite::readDataCached(const QString &cacheFileName)
{
    qInfo("Read adjustment data...");
    if(m_adjDataReadIsValid) {
        qInfo("Adjustment data was not changed since last valid read. No read necessary.");
        return true;
    }
    m_adjDataReadIsValid = false;
    QByteArray ba;
    quint32 sizeRead;
    bool readOk = false;
    if(readSizeAndChecksum(sizeRead)) {
        if(readAllAndValidateFromCache(ba, sizeRead, cacheFileName))
            readOk = true;
        else if(readAllAndValidateFromChip(ba, sizeRead)) {
            readOk = true;
            writeRawDataToCache(ba, cacheFileName);
        }
    }
    if(readOk) {
        m_adjData = ba;
        m_adjDataReadIsValid = true;
    }
    else
        qWarning("Read adjustment data failed!");
    return m_adjDataReadIsValid;
}

bool AdjustmentEepromReadWrite::writeData()
{
    // There are different places not properly designed
    // readData is bound to decode so to make writes take effect
    // reading is mandatory. So force a read
    m_adjDataReadIsValid = false;
    setCountAndChecksum(m_adjData);
    return writeRawDataToChip(m_adjData);
}

bool AdjustmentEepromReadWrite::resetData()
{
    m_adjDataReadIsValid = false;
    return m_adjMemory->Reset() == m_adjMemory->getByteSize();
}

quint32 AdjustmentEepromReadWrite::getMaxSize()
{
    return m_adjMemory->getByteSize();
}

QByteArray AdjustmentEepromReadWrite::getData()
{
    return m_adjData;
}

void AdjustmentEepromReadWrite::setData(const QByteArray &ba)
{
    m_adjData = ba;
}

quint16 AdjustmentEepromReadWrite::getChecksum()
{
    return m_checksum;
}

bool AdjustmentEepromReadWrite::readSizeAndChecksum(quint32 &sizeRead)
{
    const int headerLen = sizeof(sizeRead) + sizeof(m_checksum);
    QByteArray ba;
    ba.resize(headerLen);
    int bytesRead = m_adjMemory->ReadData(ba.data(), headerLen, 0);
    if(bytesRead != headerLen) {
        qCritical("Error on flash read: expected: %i / read %i", headerLen, bytesRead);
        return false;
    }
    QDataStream bastream(&ba, QIODevice::ReadOnly);
    bastream.setVersion(QDataStream::Qt_5_4);
    bastream >> sizeRead >> m_checksum;
    quint32 memSize = m_adjMemory->getByteSize();
    if(sizeRead > memSize) {
        qWarning("EEPROM size wanted: %u is larger than available %u - fresh EEPROM?", sizeRead, memSize);
        return false;
    }
    return true;
}

bool AdjustmentEepromReadWrite::readAllAndValidateFromChip(QByteArray &ba, quint32 size)
{
    qInfo("Read adjustment data from chip...");
    ba.resize(size);
    quint32 sizeRead = m_adjMemory->ReadData(ba.data(), size, 0);
    if (sizeRead < size) {
        qCritical("Error on chip adjustment data read: wanted: %i / available %i / read %i",
                  size, m_adjMemory->getByteSize(), sizeRead);
        return false;
    }
    // for checksum calculation checksum in buffer is set 0
    // TODO?: solve different states
    // * after read: checksum = 0
    // * after write: checksum as calculated...
    setChecksumInBuffer(ba, 0);
    quint16 checksum = qChecksum(ba.data(), ba.size());
    if(checksum != m_checksum) {
        qWarning("Chip adjustment data has incorrect checksum: expected 0x%04X / found 0x%04X", m_checksum, checksum);
        return false;
    }
    qInfo("Chip adjustment data read is valid.");
    return true;
}

bool AdjustmentEepromReadWrite::readAllAndValidateFromCache(QByteArray &ba, quint32 size, QString cacheFileName)
{
    if(cacheFileName.isEmpty())
        return false;
    qInfo("Try read adjustment data from cache...");
    QFile cacheFile(getCacheFullFileName(cacheFileName));
    if(cacheFile.open(QIODevice::ReadOnly)) {
        QByteArray cacheData = cacheFile.readAll();
        if(size == (quint32)cacheData.size()) {
            setChecksumInBuffer(cacheData, 0);
            quint16 checksum = qChecksum(cacheData.data(), cacheData.size());
            if(checksum == m_checksum) {
                qInfo("Read adjustment data from cache passed.");
                setChecksumInBuffer(cacheData, checksum);
                ba = cacheData;
                return true;
            }
            else
                qWarning("Cache file has incorrect checksum: expected 0x%04X / found 0x%04X", m_checksum, checksum);
        }
        else
            qWarning("Cache file has incorrect size: expected %u / found %u", size, cacheData.size());
    }
    else
        qWarning("Cannot open cache file %s!", qPrintable(cacheFile.fileName()));
    return false;
}

bool AdjustmentEepromReadWrite::writeRawDataToChip(QByteArray &ba)
{
    int count = ba.size();
    int written = m_adjMemory->WriteData(ba.data(), count, 0);
    return count == written;
}

void AdjustmentEepromReadWrite::writeRawDataToCache(QByteArray ba, QString cacheFileName)
{
    if(cacheFileName.isEmpty())
        return;
    qInfo("Write adjustment cache file...");
    QDir dir;
    if(!dir.mkpath(m_cachePath)) {
        qWarning("Could not create cache path %s!", qPrintable(m_cachePath));
        return;
    }
    QString fullFileName = getCacheFullFileName(cacheFileName);
    QFile::remove(fullFileName); // cache files are read-only
    QFile file(fullFileName);
    if(file.open(QIODevice::WriteOnly)) {
        setCountAndChecksum(ba);
        if(file.write(ba) == ba.size())
            qInfo("Adjustment cache file written sucessfully.");
        else
            qWarning("Adjustment cache was not written completely!");
    }
    else
        qWarning("Could not create cache file %s!", qPrintable(fullFileName));
}

void AdjustmentEepromReadWrite::setCountAndChecksum(QByteArray &ba)
{
    setCountInBuffer(ba);
    setChecksumInBuffer(ba, 0);
    m_checksum = qChecksum(ba.data(), ba.size());
    setChecksumInBuffer(ba, m_checksum);
}

void AdjustmentEepromReadWrite::setChecksumInBuffer(QByteArray &ba, quint16 checksum)
{
    QDataStream stream(&ba, QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_4);
    stream.skipRawData(sizeof(quint32 /* size field */));
    stream << checksum;
}

void AdjustmentEepromReadWrite::setCountInBuffer(QByteArray &ba)
{
    quint32 count = ba.size();
    QDataStream stream(&ba, QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_4);
    stream << count;
}
