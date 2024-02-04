#ifndef ABSTRACTDSPDEVICENODE_H
#define ABSTRACTDSPDEVICENODE_H

#include <QString>
#include <memory>

class AbstractDspDeviceNode
{
public:
    virtual ~AbstractDspDeviceNode() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;
    virtual bool dspReset() = 0;
    virtual bool dspBoot(QString bootFileName) = 0;
    virtual int dspRequestInt() = 0;
    virtual int dspGetMagicId() = 0;
    virtual bool dspIsRunning() = 0;
    virtual int lcaRawVersion() = 0;

    virtual int lseek(ulong adr) = 0;
    virtual bool write(ulong adr, const char* buf, int len) = 0;
    virtual int read(char* buf, int len) = 0;
    virtual void enableFasync() = 0;
};

typedef std::shared_ptr<AbstractDspDeviceNode> AbstractDspDeviceNodePtr;

#endif // ABSTRACTDSPDEVICENODE_H
