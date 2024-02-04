#ifndef DEMODEVICENODEDSP_H
#define DEMODEVICENODEDSP_H

#include "abstractdspdevicenode.h"

class DemoDeviceNodeDsp : public AbstractDspDeviceNode
{
public:
    DemoDeviceNodeDsp(int dspMagicId);
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

#endif // DEMODEVICENODEDSP_H
