#ifndef ABSTRACTLOGVALUEGETTER_H
#define ABSTRACTLOGVALUEGETTER_H

template <class T>
class AbstractLogValueGetter
{
public:
    virtual ~AbstractLogValueGetter() = default;
    virtual bool canGetValue() = 0;
    virtual T getValue() = 0;
};

#endif // ABSTRACTLOGVALUEGETTER_H
