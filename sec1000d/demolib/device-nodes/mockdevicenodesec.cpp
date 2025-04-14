#include "mockdevicenodesec.h"
#include <QJsonObject>

int MockDeviceNodeSec::open(QString devNodeFileName)
{
    m_deviceNodeRecorder.open(devNodeFileName);
    return 0;
}

void MockDeviceNodeSec::close()
{
    m_deviceNodeRecorder.close();
}

int MockDeviceNodeSec::lseek(ulong adr)
{
    m_deviceNodeRecorder.lseek(adr);
    return 0;
}

int MockDeviceNodeSec::write(const char *buf, int len)
{
    m_deviceNodeRecorder.write(buf, len);
    return 0;
}

int MockDeviceNodeSec::read(char *buf, int len)
{
    m_deviceNodeRecorder.read(buf, len);
    if(!m_nextReadValue.isEmpty()) {
        for (int byteNo = 0; byteNo<len; byteNo++)
            buf[byteNo] = m_nextReadValue[byteNo];
        m_nextReadValue.clear();
    }
    return 0;
}

void MockDeviceNodeSec::enableFasync()
{
    m_deviceNodeRecorder.enableFasync();
}

QJsonObject MockDeviceNodeSec::getRecording()
{
    return m_deviceNodeRecorder.getRecording();
}

void MockDeviceNodeSec::resetRecording()
{
    m_deviceNodeRecorder.resetRecording();
}

void MockDeviceNodeSec::setNextReadValue(QByteArray nextReadValue)
{
    m_nextReadValue = nextReadValue;
}
