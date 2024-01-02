#ifndef ATMELPERMISSIONTEMPLATE_H
#define ATMELPERMISSIONTEMPLATE_H

#include <zeramcontrollerio.h>
#include <memory.h>

class AtmelPermissionTemplate
{
public:
    virtual ~AtmelPermissionTemplate() = default;
    virtual ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) = 0;
    bool /*success*/ hasPermission(bool &allow);
};

typedef std::unique_ptr<AtmelPermissionTemplate> AtmelPermissionTemplatePtrU;

#endif // ATMELPERMISSIONTEMPLATE_H
