#ifndef DSPVAR_H
#define DSPVAR_H

#include <QString>
#include <QVector>

namespace DSPDATA
{
    enum dType {dInt, dFloat}; // data value type int or float

    enum segmentType { localSegment, globalSegment};

    enum DspValueType {vDspResult = 1, vDspTemp = 2, vDspIntVar = 4, vDspParam = 8, vDspALL = 15, vDspTempGlobal = 16};
}

class cDspVar // dsp variable
{
public:
    QString& Name() { return m_sName;}
    int size() { return m_dspVarData.size(); }
    int type() { return m_nType; }
    int datatype() { return m_nDataType; }
    void setValue(int idx, float value);
    // Nightmare: This must go!!!
    float* data() { return m_dspVarData.data();}

    static int getInstanceCount();
private:
    friend class cDspMeasData;
    cDspVar(const QString &name, int size, int type, int datatype = DSPDATA::dFloat );
    virtual ~cDspVar();
    void setData(QVector<float> data) {
        Q_ASSERT(m_dspVarData.size() == data.size());
        m_dspVarData = data;
    }

    QString m_sName; // a var. has its name
    int m_nType; // an it can be of different type : vDspResult, vDspTemp , vDspIntVar , vDspParam
    int m_nDataType; // it can be float or int
    QVector<float> m_dspVarData; // we hold an array for data storage
    static int m_instanceCount;
};
#endif // DSPVAR_H
