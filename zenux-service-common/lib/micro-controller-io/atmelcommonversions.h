#ifndef ATMELCOMMONVERSIONS_H
#define ATMELCOMMONVERSIONS_H

#include <zera_mcontroller_base.h>
#include <memory>

class AtmelCommonVersions
{
public:
    virtual ~AtmelCommonVersions() = default;

    virtual ZeraMcontrollerBase::atmelRM readCTRLVersion(QString& answer) = 0;
    virtual ZeraMcontrollerBase::atmelRM readPCBVersion(QString& answer) = 0;
};

typedef std::shared_ptr<AtmelCommonVersions> AtmelCommonVersionsPtr;

#endif // ATMELCOMMONVERSIONS_H
