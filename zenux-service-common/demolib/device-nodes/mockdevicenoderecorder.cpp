#include "mockdevicenoderecorder.h"

void MockDeviceNodeRecorder::open(const QString &devNodeFileName)
{
    QJsonObject dumpEntry;
    dumpEntry.insert("open", devNodeFileName);
    m_dumpedIos.append(dumpEntry);
}

void MockDeviceNodeRecorder::close()
{
    QJsonObject dumpEntry;
    dumpEntry.insert("close", "");
    m_dumpedIos.append(dumpEntry);
}

void MockDeviceNodeRecorder::lseek(ulong adr)
{
    QJsonObject dumpEntry;
    QString hexAddr = QString("0x%1").arg(adr, 8, 16, QChar('0'));
    dumpEntry.insert("lseek", hexAddr);
    m_dumpedIos.append(dumpEntry);
}

void MockDeviceNodeRecorder::write(const char *buf, int len)
{
    Q_UNUSED(buf)
    QJsonObject dumpEntry;
    dumpEntry.insert("write", int(len));
    m_dumpedIos.append(dumpEntry);
}

void MockDeviceNodeRecorder::read(char *buf, int len)
{
    Q_UNUSED(buf)
    QJsonObject dumpEntry;
    dumpEntry.insert("read", int(len));
    m_dumpedIos.append(dumpEntry);
}

void MockDeviceNodeRecorder::enableFasync()
{
    QJsonObject dumpEntry;
    dumpEntry.insert("enableFasync", "");
    m_dumpedIos.append(dumpEntry);
}

QJsonObject MockDeviceNodeRecorder::getRecording()
{
    QJsonObject ret;
    ret.insert("Recordings", m_dumpedIos);
    return ret;
}

void MockDeviceNodeRecorder::resetRecording()
{
    m_dumpedIos = QJsonArray();
}
