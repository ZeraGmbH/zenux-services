#ifndef DSPVARRESOLVER_H
#define DSPVARRESOLVER_H

#include "dspmemorysectioninternal.h"
#include <QString>
#include <QHash>

class DspVarResolver
{
public:
    DspVarResolver();
    void addSection(DspMemorySectionInternal* section);
    void actualizeVarHash();
    DspVarServerPtr getDspVar(const QString& varNameWithOffset);
    long getVarOffset(const QString &varNameWithOffset);
    long getVarAddress(const QString& varNameWithOffset);
    int getVarType(const QString &varNameWithOffset);

private:
    QHash<QString, DspVarServerPtr> m_varHash;
    QList<DspMemorySectionInternal*> m_memSectionList;
};

#endif // DSPVARRESOLVER_H
