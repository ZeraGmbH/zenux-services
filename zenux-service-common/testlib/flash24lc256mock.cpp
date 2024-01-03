#include "flash24lc256mock.h"

static constexpr int sizeFlash = 32768;

QHash<QString, QHash<short, QByteArray>> Flash24LC256Mock::m_flashData;

Flash24LC256Mock::Flash24LC256Mock(QString devNode, short i2cAddr) :
    m_devNode(devNode),
    m_i2cAddr(i2cAddr)
{
    if(m_flashData.contains(devNode))
        return;
    if(m_flashData[devNode].contains(i2cAddr))
        return;
    doReset();
}

int Flash24LC256Mock::WriteData(char *data, ushort count, ushort adr)
{
    if(adr != 0)
        qFatal("Address other than 0 is not yet supported!");

    int toWrite = std::min(size(), int(count));
    QByteArray &flashEntry = m_flashData[m_devNode][m_i2cAddr];
    for(int i=0; i<toWrite; i++)
        flashEntry[i] = data[i];
    return toWrite;
}

int Flash24LC256Mock::ReadData(char *data, ushort count, ushort adr)
{
    if(adr != 0)
        qFatal("Address other than 0 is not yet supported!");

    int toRead = std::min(size(), int(count));
    const QByteArray flashEntry = m_flashData[m_devNode][m_i2cAddr];
    for(int i=0; i<toRead; i++)
        data[i] = flashEntry[i];
    return toRead;
}

int Flash24LC256Mock::Reset()
{
    doReset();
    return size();
}

int Flash24LC256Mock::size()
{
    return sizeFlash;
}

void Flash24LC256Mock::cleanAll()
{
    m_flashData.clear();
}

QByteArray Flash24LC256Mock::getData(QString devNode, short adr)
{
    QByteArray ret;
    if(m_flashData.contains(devNode))
        if(m_flashData[devNode].contains(adr))
            ret = m_flashData[devNode][adr];
    return ret;
}

void Flash24LC256Mock::doReset()
{
    m_flashData[m_devNode][m_i2cAddr] = QByteArray(sizeFlash, 0xff);
}
