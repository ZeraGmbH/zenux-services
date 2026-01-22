#ifndef DSPMEASDATA_H
#define DSPMEASDATA_H

#include <QString>
#include <QList>
#include <QVector>
#include "dspvar.h"

class cDspMeasData
{
public:
    cDspMeasData(QString name);
    ~cDspMeasData();

    cDspVar *addDspVar(const QString &name, int size, int type, int datatype = DSPDATA::dFloat);

    // Nightmare candidate: Make it go!!!
    float* data(const QString &name);
    void setVarData(const QString &datalist); // expected 'Name1:0.2,0.4,0,3;Name2:1,2,3;' ......

    QString getName();
    quint32 getSize();
    quint32 getUserMemSize();
    QString VarListLong(int section);
    QString VarListShort(int section);
    QString writeCommand();
    QVector<float>& getData();

    // Test insights
    const QList<cDspVar*> getVars() const;
    void setData(const QVector<float> &data);
private:
    cDspVar* findVar(const QString &varName);

    QString m_handleName;
    QList<cDspVar*> DspVarList;
    QVector<float> vector;
};

#endif // DSPMEASDATA_H
