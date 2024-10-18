#ifndef LogCollectorFloatingNumbers_H
#define LogCollectorFloatingNumbers_H

#include "abstractlogcollector.h"

template <class T>
class LogCollectorFloatingNumbers : public AbstractLogCollector<T>
{
public:
    LogCollectorFloatingNumbers(int maxSize) : AbstractLogCollector<T>(maxSize) {}
    bool isValid(const T &value) override {
        return !qIsInf(value) && !qIsNaN(value);
    }
};

#endif // LogCollectorFloatingNumbers_H
