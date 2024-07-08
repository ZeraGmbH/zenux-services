#ifndef LOGSTRATEGY_H
#define LOGSTRATEGY_H

#include <QList>

class LogStrategy
{
public:
    LogStrategy();
    void addValue(QList<float> &values, float newValue);
};

#endif // LOGSTRATEGY_H
