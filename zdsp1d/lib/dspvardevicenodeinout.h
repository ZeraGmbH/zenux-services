#ifndef DSPVARDEVICENODEINOUT_H
#define DSPVARDEVICENODEINOUT_H

#include "abstractfactorydevicenodedsp.h"
#include "dspvardefinitions.h"
#include "dspvarresolver.h"

class DspVarDeviceNodeInOut
{
public:
    DspVarDeviceNodeInOut(AbstractFactoryDeviceNodeDspPtr deviceNodeFactory);
    TDspVar *readOneDspVar(const QString &nameCommaLen, QByteArray *varRead, DspVarResolver *dspVarResolver);
    bool doReadVarFromDsp(TDspVar *DspVar, int countVars, QByteArray *varRead);
    bool doWriteDspVars(const QString &varsSemicolonSeparated, DspVarResolver *dspVarResolver);
private:
    static bool tryStreamIntegerValue(const QString &strValue, QDataStream &stream);
    static bool tryStreamFloatValue(const QString &strValue, QDataStream &stream);
    AbstractFactoryDeviceNodeDspPtr m_deviceNodeFactory;
};

#endif // DSPVARDEVICENODEINOUT_H
