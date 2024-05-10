#ifndef MOCKEEPROM24LC_H
#define MOCKEEPROM24LC_H

#include <i2cflashinterface.h>
#include <QHash>

class MockEEprom24LC : public I2cFlashInterface
{
public:
    MockEEprom24LC(QString devNode, short i2cAddr);
    int WriteData(char* data, ushort count, ushort adr) override;
    int ReadData(char* data, ushort count, ushort adr) override;
    virtual int Reset() override;
    virtual int size() override;

    void returnReduceCountOnErrorRead();
    static void mockCleanAll();
    static QByteArray mockGetData(QString devNode, short adr);
    static void mockSetData(QString devNode, short adr, QByteArray data);
    static int mockGetReadCount(QString devNode, short adr);
    static int mockGetWriteCount(QString devNode, short adr);
    static bool mockWriteToFile(QString devNode, short adr, QString fileName);
    static bool mockReadFromFile(QString devNode, short adr, QString fileName);
    static bool mockCompareWithFile(QString devNode, short adr, QString fileName);
private:
    void doReset(int size);
    QString m_devNode;
    short m_i2cAddr;
    static QHash<QString, QHash<short, QByteArray>> m_flashData;
    static QHash<QString, QHash<short, int>> m_flashDataReadCounts;
    static QHash<QString, QHash<short, int>> m_flashDataWriteCounts;
    static QHash<QString, QHash<short, bool>> m_returnReducedDataSizeOnRead;
};

#endif // MOCKEEPROM24LC_H
