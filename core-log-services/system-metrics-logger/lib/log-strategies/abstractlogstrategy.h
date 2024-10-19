#ifndef ABSTRACTLOGSTRATEGY_H
#define ABSTRACTLOGSTRATEGY_H

#include <QList>

template <class T>
class AbstractLogStrategy
{
public:
    virtual void addValue(T newValue) = 0;
};

#endif // ABSTRACTLOGSTRATEGY_H
