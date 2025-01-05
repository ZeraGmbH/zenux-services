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
    bool readOneDspVarInt(const QString &varName, int& intval, DspVarResolver *dspVarResolver);
    QString readDspVarList(const QString &variablesString, DspVarResolver *dspVarResolver); // format: '<name1>,<len1>;<name2>,<len2>'
    bool writeDspVars(const QString &varsSemicolonSeparated, DspVarResolver *dspVarResolver);
private:
    bool readVarFromDsp(TDspVar *DspVar, int countVars, QByteArray *varRead);
    static bool tryStreamIntegerValue(const QString &strValue, QDataStream &stream);
    static bool tryStreamFloatValue(const QString &strValue, QDataStream &stream);
    AbstractFactoryDeviceNodeDspPtr m_deviceNodeFactory;
};

#endif // DSPVARDEVICENODEINOUT_H
