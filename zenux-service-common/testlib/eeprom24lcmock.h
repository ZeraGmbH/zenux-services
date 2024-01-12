#ifndef EEPROM24LCMOCK_H
#define EEPROM24LCMOCK_H

#include <i2cflashinterface.h>
#include <QHash>

class EEprom24LCMock : public I2cFlashInterface
{
public:
    EEprom24LCMock(QString devNode, short i2cAddr);
    int WriteData(char* data, ushort count, ushort adr) override;
    int ReadData(char* data, ushort count, ushort adr) override;
    virtual int Reset() override;
    virtual int size() override;

    void returnReduceCountOnErrorRead();
    static void cleanAll();
    static QByteArray getData(QString devNode, short adr);
    static int getWriteCount(QString devNode, short adr);
private:
    void doReset(int size);
    QString m_devNode;
    short m_i2cAddr;
    static QHash<QString, QHash<short, QByteArray>> m_flashData;
    static QHash<QString, QHash<short, int>> m_flashDataWriteCounts;
    static QHash<QString, QHash<short, bool>> m_returnReducedDataSizeOnRead;
};

#endif // EEPROM24LCMOCK_H
