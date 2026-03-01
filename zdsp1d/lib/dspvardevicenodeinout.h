#ifndef DSPVARDEVICENODEINOUT_H
#define DSPVARDEVICENODEINOUT_H

#include "abstractfactoryzdspsupport.h"
#include "dspvarresolver.h"

class DspVarDeviceNodeInOut
{
public:
    DspVarDeviceNodeInOut(AbstractFactoryZdspSupportPtr zdspSupportFactory);

    DspVarServer *readOneDspVar(const QString &nameCommaLen, QByteArray *varRead, DspVarResolver *dspVarResolver);
    bool readOneDspVarInt(const QString &varName, int& intval, DspVarResolver *dspVarResolver);
    QString readDspVarList(const QString &variablesString, DspVarResolver *dspVarResolver); // format: '<name1>,<len1>;<name2>,<len2>'
    bool writeDspVars(const QString &varsSemicolonSeparated, DspVarResolver *dspVarResolver);
private:
    bool readVarFromDsp(DspVarServer *DspVar, int countVars, QByteArray *varRead);
    static bool tryStreamIntegerValue(const QString &strValue, QDataStream &stream);
    static bool tryStreamFloatValue(const QString &strValue, QDataStream &stream);
    AbstractFactoryZdspSupportPtr m_zdspSupportFactory;
};

#endif // DSPVARDEVICENODEINOUT_H
