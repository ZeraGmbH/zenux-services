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
    float* data(QString name);
    void addVarItem(cDspVar*);
    quint32 getSize(); // number of all element in this container
    quint32 getSize(QString name); // number of elements of var name in this container
    quint32 getumemSize();
    QString& VarListLong(int section);
    QString& VarListShort(int section);
    QString& writeCommand();
    QVector<float>& getData();

private:
    QString m_handleName;
    QList<cDspVar*> DspVarList;
    QVector<float> vector;
    QString sReturn;
};

#endif // DSPMEASDATA_H
