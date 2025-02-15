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
    cDspVar(QString name, int size, int type, int datatype = DSPDATA::dFloat ) :
        m_sName(name),
        m_nsize(size),
        m_nType(type),
        m_nDataType(datatype)
    {
        // we only reserve memory for data of interrest
        if ( (type & (DSPDATA::vDspResult | DSPDATA::vDspIntVar | DSPDATA::vDspParam)) > 0)
            m_dspVarData.resize(size);
    }
    QString& Name() { return m_sName;}
    int size() { return m_nsize; }
    int type() { return m_nType; }
    int datatype() { return m_nDataType; }
    float* data() { return m_dspVarData.data();}

private:
    friend class cDspMeasData;
    void setData(QVector<float> data) {
        Q_ASSERT(m_dspVarData.size() == data.size());
        m_dspVarData = data;
    }

    QString m_sName; // a var. has its name
    int m_nsize; // it has a number of elements
    int m_nType; // an it can be of different type : vDspResult, vDspTemp , vDspIntVar , vDspParam
    int m_nDataType; // it can be float or int
    QVector<float> m_dspVarData; // we hold an array for data storage
};
#endif // DSPVAR_H
