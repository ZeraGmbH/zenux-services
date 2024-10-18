#ifndef ABSTRACTLOGSTRATEGY_H
#define ABSTRACTLOGSTRATEGY_H

#include <QList>

class AbstractLogStrategy
{
public:
    virtual void addValue(QList<float> &values, float newValue) = 0;
};

#endif // ABSTRACTLOGSTRATEGY_H
