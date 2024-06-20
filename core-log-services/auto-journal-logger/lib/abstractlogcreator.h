#ifndef ABSTRACTLOGCREATOR_H
#define ABSTRACTLOGCREATOR_H

#include <QString>
#include <memory>

class AbstractLogCreator
{
public:
    virtual bool storeLogs(QString dir) = 0;
};

typedef std::unique_ptr<AbstractLogCreator> AbstractLogCreatorPtr;

#endif // ABSTRACTLOGCREATOR_H
