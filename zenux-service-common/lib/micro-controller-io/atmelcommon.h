#ifndef ATMELCOMMON_H
#define ATMELCOMMON_H

#include <zera_mcontroller_base.h>

class AtmelCommon
{
public:
    virtual ~AtmelCommon() = default;
    virtual ZeraMcontrollerBase::atmelRM readCTRLVersion(QString& answer) = 0;
};

#endif // ATMELCOMMON_H
