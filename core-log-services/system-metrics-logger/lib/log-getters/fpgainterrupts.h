#ifndef FPGAINTERRUPTS_H
#define FPGAINTERRUPTS_H

#include "abstractlogvaluegetter.h"
#include "interruptvalues.h"

class FpgaInterrupts : public AbstractLogValueGetter<float>
{
public:
    bool canGetValue() override;
    float getValue() override;

private:
    InterruptValues m_previousInterrupts;
};

#endif // FPGAINTERRUPTS_H
