#ifndef DSPVARGROUPCLIENTINTERFACE_H
#define DSPVARGROUPCLIENTINTERFACE_H

#include <QString>
#include <QList>
#include <QVector>
#include "dspvarclientinterface.h"

class DspVarGroupClientInterface
{
public:
    DspVarGroupClientInterface(const QString &groupName);
    ~DspVarGroupClientInterface();

    DspVarClientInterface *addDspVar(const QString &varName, int size,
                       DspDataType dataType = dspDataTypeFloat, DspSegmentType dspSegmentType = moduleLocalSegment);

    // Nightmare candidate: Make it go!!!
    float* data(QString name);
    void setVarData(QString datalist); // expected 'Name1:0.2,0.4,0,3;Name2:1,2,3;' ......

    QString getName();
    quint32 getUserMemSize();
    quint32 getUserMemSizeGlobal();
    QString VarListLong();
    QString VarListShort();
    QString writeCommand();
    QVector<float>& getData();

    // Test insights
    static int getInstanceCount();
    const QList<DspVarClientInterface*> getVars() const;
    void setData(const QVector<float> &data);
private:
    DspVarClientInterface* findVar(const QString &varName);

    QString m_groupName;
    QList<DspVarClientInterface*> DspVarList;
    QVector<float> vector;

    static int m_instanceCount;
};

#endif // DSPVARGROUPCLIENTINTERFACE_H
