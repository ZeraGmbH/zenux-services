#ifndef ABSTRACTLOGCREATOR_H
#define ABSTRACTLOGCREATOR_H

#include <memory>

class AbstractLogCreator
{
public:
    virtual bool storeLogs() = 0;
};

typedef std::unique_ptr<AbstractLogCreator> AbstractLogCreatorPtr;

#endif // ABSTRACTLOGCREATOR_H
