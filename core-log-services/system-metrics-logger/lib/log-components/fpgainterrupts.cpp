#include "fpgainterrupts.h"
#include "procinterruptdecoder.h"

bool FpgaInterrupts::canGetValue()
{
    InterruptValues interruptValues = ProcInterruptDecoder::getCurrentInterrupts();
    if(!interruptValues.isEmpty)
        return true;
    return false;
}

float FpgaInterrupts::getValue()
{
    InterruptValues interruptValues = ProcInterruptDecoder::getCurrentInterrupts();
    quint32 interruptsSinceLast = interruptValues.fpgaInterrupts - m_previousInterrupts.fpgaInterrupts;
    m_previousInterrupts.fpgaInterrupts = interruptValues.fpgaInterrupts;
    return interruptsSinceLast;
}

