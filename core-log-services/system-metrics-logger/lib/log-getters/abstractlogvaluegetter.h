#ifndef ABSTRACTLOGVALUEGETTER_H
#define ABSTRACTLOGVALUEGETTER_H

class AbstractLogValueGetter
{
public:
    virtual bool canGetValue() = 0;
    virtual float getValue() = 0;
};

#endif // ABSTRACTLOGVALUEGETTER_H
