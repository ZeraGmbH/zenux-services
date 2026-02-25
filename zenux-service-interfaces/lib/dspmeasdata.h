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
    float* data(QString name);
    void setVarData(QString datalist); // expected 'Name1:0.2,0.4,0,3;Name2:1,2,3;' ......

    QString getName();
    quint32 getUserMemSize();
    quint32 getUserMemSizeGlobal();
    QString VarListLong(int section);
    QString VarListShort(int section);
    QString writeCommand();
    QVector<float>& getData();

    // Test insights
    static int getInstanceCount();
    const QList<cDspVar*> getVars() const;
    void setData(QVector<float> data);
private:
    cDspVar* findVar(QString varName);

    QString m_handleName;
    QList<cDspVar*> DspVarList;
    QVector<float> vector;

    static int m_instanceCount;
};

#endif // DSPMEASDATA_H
