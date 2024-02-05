#ifndef TESTDEVICENODEDSP_H
#define TESTDEVICENODEDSP_H

#include "abstractdspdevicenode.h"
#include <QObject>
#include <QVariant>

class TestDeviceNodeDsp : public QObject, public AbstractDspDeviceNode
{
    Q_OBJECT
public:
    TestDeviceNodeDsp(int dspMagicId);
    bool dspReset() override;
    bool dspBoot(QString bootFileName) override;
    int dspRequestInt() override;
    int dspGetMagicId() override;
    bool dspIsRunning() override;
    int lcaRawVersion() override;

    int open(QString devNodeFileName) override;
    void close() override;
    int lseek(ulong adr) override;
    bool write(ulong adr, const char* buf, int len) override;
    int read(char* buf, int len) override;
    void enableFasync() override;

signals:
    void sigIoOperation(QString funcName, QVariant par1 = QVariant(), QVariant par2 = QVariant(), QVariant par3 = QVariant());
private:
    int m_dspMagicId;
};

typedef std::shared_ptr<TestDeviceNodeDsp> TestDeviceNodeDspPtr;

#endif // TESTDEVICENODEDSP_H
