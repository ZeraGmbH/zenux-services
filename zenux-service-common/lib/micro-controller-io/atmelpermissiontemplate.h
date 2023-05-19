#ifndef ATMELPERMISSIONTEMPLATE_H
#define ATMELPERMISSIONTEMPLATE_H

#include <zeramcontrollerio.h>

class AtmelPermissionTemplate
{
public:
    virtual ~AtmelPermissionTemplate() = default;
    virtual ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) = 0;
    bool /*success*/ hasPermission(bool &allow);
};

#endif // ATMELPERMISSIONTEMPLATE_H
