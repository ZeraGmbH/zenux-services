#ifndef MOCKATMEL_H
#define MOCKATMEL_H

#include <atmelpermissiontemplate.h>

class MockAtmel : public AtmelPermissionTemplate
{
public:
    ZeraMcontrollerBase::atmelRM getEEPROMAccessEnable(bool &enable) override;
};

#endif // MOCKATMEL_H
