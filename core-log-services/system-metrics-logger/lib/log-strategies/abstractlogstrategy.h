#ifndef ABSTRACTLOGSTRATEGY_H
#define ABSTRACTLOGSTRATEGY_H

#include <QList>

class AbstractLogStrategy
{
public:
    virtual void addValue(float newValue) = 0;
};

#endif // ABSTRACTLOGSTRATEGY_H
