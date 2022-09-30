#ifndef ATMELBASEINTERFACE_H
#define ATMELBASEINTERFACE_H

#include <zera_mcontroller_base.h>

class AtmelBaseInterface
{
public:
    virtual ZeraMcontrollerBase::atmelRM readCTRLVersion(QString& answer) = 0;
};

#endif // ATMELBASEINTERFACE_H
