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

    void addVarItem(cDspVar*);

    // Nightmare candidate: Make it go!!!
    float* data(QString name);
    void setVarData(QString datalist); // expected 'Name1:0.2,0.4,0,3;Name2:1,2,3;' ......

    QString getName();
    quint32 getSize();
    quint32 getUserMemSize();
    QString VarListLong(int section);
    QString VarListShort(int section);
    QString writeCommand();
    QVector<float>& getData();

    // Test insights
    const QList<cDspVar*> getVars() const;
private:
    cDspVar* findVar(QString varName);
    friend class MockDspInterface;
    void setData(QVector<float> data);

    QString m_handleName;
    QList<cDspVar*> DspVarList;
    QVector<float> vector;
};

#endif // DSPMEASDATA_H
