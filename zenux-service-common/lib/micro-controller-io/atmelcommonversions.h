#ifndef ATMELCOMMONVERSIONS_H
#define ATMELCOMMONVERSIONS_H

#include <zeramcontrollerio.h>
#include <memory>

class AtmelCommonVersions
{
public:
    virtual ~AtmelCommonVersions() = default;

    virtual ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM readPCBVersion(QString& answer) = 0;
};

typedef std::shared_ptr<AtmelCommonVersions> AtmelCommonVersionsPtrS;
typedef std::unique_ptr<AtmelCommonVersions> AtmelCommonVersionsPtrU;

#endif // ATMELCOMMONVERSIONS_H
