#include "mockdevicenodesec.h"
#include <QJsonObject>

int MockDeviceNodeSec::open(QString devNodeFileName)
{
    QJsonObject dumpEntry;
    dumpEntry.insert("open", devNodeFileName);
    m_dumpedIos.append(dumpEntry);
    return 0;
}

void MockDeviceNodeSec::close()
{
    QJsonObject dumpEntry;
    dumpEntry.insert("close", "");
    m_dumpedIos.append(dumpEntry);
}

int MockDeviceNodeSec::lseek(ulong adr)
{
    QJsonObject dumpEntry;
    QString hexAddr = QString("0x%1").arg(adr, 8, 16, QChar('0'));
    dumpEntry.insert("lseek", hexAddr);
    m_dumpedIos.append(dumpEntry);
    return 0;
}

int MockDeviceNodeSec::write(const char *buf, int len)
{
    Q_UNUSED(buf)
    QJsonObject dumpEntry;
    dumpEntry.insert("write", int(len));
    m_dumpedIos.append(dumpEntry);
    return 0;
}

int MockDeviceNodeSec::read(char *buf, int len)
{
    Q_UNUSED(buf)
    QJsonObject dumpEntry;
    dumpEntry.insert("read", int(len));
    m_dumpedIos.append(dumpEntry);
    return 0;
}

void MockDeviceNodeSec::enableFasync()
{
    QJsonObject dumpEntry;
    dumpEntry.insert("enableFasync", "");
    m_dumpedIos.append(dumpEntry);
}

QJsonObject MockDeviceNodeSec::getRecording()
{
    QJsonObject ret;
    ret.insert("Recordings", m_dumpedIos);
    return ret;
}

void MockDeviceNodeSec::resetRecording()
{
    m_dumpedIos = QJsonArray();
}
