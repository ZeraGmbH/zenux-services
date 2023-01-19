#ifndef ATMELISEEACCESSENABLE_H
#define ATMELISEEACCESSENABLE_H

#include <zera_mcontroller_base.h>

class AtmelIsEEAccessEnable
{
public:
    virtual ZeraMcontrollerBase::atmelRM getEEPROMAccessEnable(bool &enable) = 0;
};

#endif // ATMELISEEACCESSENABLE_H
