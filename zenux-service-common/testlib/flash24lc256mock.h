#ifndef FLASH24LC256MOCK_H
#define FLASH24LC256MOCK_H

#include <i2cflashinterface.h>
#include <QHash>

class Flash24LC256Mock : public I2cFlashInterface
{
public:
    Flash24LC256Mock(QString devNode, short i2cAddr);
    int WriteData(char* data, ushort count, ushort adr) override;
    int ReadData(char* data, ushort count, ushort adr) override;
    virtual int Reset() override;
    virtual int size() override;

    static void cleanAll();
    static QByteArray getData(QString devNode, short adr);
private:
    void doReset();
    QString m_devNode;
    short m_i2cAddr;
    static QHash<QString, QHash<short, QByteArray>> m_flashData;
};

#endif // FLASH24LC256MOCK_H
