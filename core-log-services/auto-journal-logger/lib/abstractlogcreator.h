#ifndef ABSTRACTLOGCREATOR_H
#define ABSTRACTLOGCREATOR_H

#include <QString>
#include <memory>

class AbstractLogCreator
{
public:
    virtual ~AbstractLogCreator() = default;
    virtual bool storeLogs(QString destinationDir) = 0;
};

typedef std::unique_ptr<AbstractLogCreator> AbstractLogCreatorPtr;

#endif // ABSTRACTLOGCREATOR_H
