#ifndef DSPVARRESOLVER_H
#define DSPVARRESOLVER_H

#include "dspmemorysectioninternal.h"
#include <parse.h>
#include <QString>
#include <QHash>

class DspVarResolver
{
public:
    DspVarResolver();
    void addSection(DspMemorySectionInternal* section);
    void actualizeVarHash();
    DspVarServerPtr getDspVar(const QString& varNameWithOffset);
    long getVarOffset(const QString &varNameWithOffset, ulong userMemOffset, ulong globalstartadr);
    long getVarAddress(const QString& varNameWithOffset);
    int getVarType(const QString &varNameWithOffset);

private:
    static QString extractOffset(const QString &varNameWithOffset, const QString &varName);
    static long calcOffsetFromStr(const QString &str);
    QHash<QString, DspVarServerPtr> m_varHash;
    cParse m_varParser;
    QList<DspMemorySectionInternal*> MemSectionList;
};

#endif // DSPVARRESOLVER_H
