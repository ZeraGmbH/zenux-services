#include "fpgainterrupts.h"
#include "procinterruptdecoder.h"

bool FpgaInterrupts::canGetValue()
{
    // on device these will turn up eventually
    return true;
}

float FpgaInterrupts::getValue()
{
    InterruptValues interruptValues = ProcInterruptDecoder::getCurrentInterrupts();
    if(!interruptValues.isEmpty) {
        quint32 interruptsSinceLast = interruptValues.fpgaInterrupts - m_previousInterrupts.fpgaInterrupts;
        m_previousInterrupts.fpgaInterrupts = interruptValues.fpgaInterrupts;
        return interruptsSinceLast;
    }
    return qQNaN();
}

