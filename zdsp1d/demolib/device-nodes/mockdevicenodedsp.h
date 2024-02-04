#ifndef MOCKDEVICENODEDSP_H
#define MOCKDEVICENODEDSP_H

#include "abstractdspdevicenode.h"

class MockDeviceNodeDsp : public AbstractDspDeviceNode
{
public:
    MockDeviceNodeDsp(int dspMagicId);
    int open(QString devNodeFileName) override;
    void close() override;
    bool dspReset() override;
    bool dspBoot(QString bootFileName) override;
    int dspRequestInt() override;
    int dspGetMagicId() override;
    bool dspIsRunning() override;
    int lcaRawVersion() override;

    int lseek(ulong adr) override;
    bool write(ulong adr, const char* buf, int len) override;
    int read(char* buf, int len) override;
    void enableFasync() override;
private:
    int m_dspMagicId;
};

#endif // MOCKDEVICENODEDSP_H
