#ifndef ATMELREADVERSIONINTERFACE_H
#define ATMELREADVERSIONINTERFACE_H

#include <zera_mcontroller_base.h>

class AtmelReadVersionInterface
{
public:
    virtual ~AtmelReadVersionInterface() = default;
    virtual ZeraMcontrollerBase::atmelRM readCTRLVersion(QString& answer) = 0;
};

#endif // ATMELREADVERSIONINTERFACE_H
