#ifndef DSPVAR_H
#define DSPVAR_H

#include <QString>
#include <QVector>

enum DspDataType { dspDataTypeInt, dspDataTypeFloat, dspDataTypeUnknown };

enum DspSegmentType { dspInternalSegment, moduleLocalSegment, moduleGlobalSegment };

namespace DSPDATA
{
    enum DspValueType { vDspResult = 1, vDspTemp = 2, vDspParam = 8, vDspALL = vDspResult | vDspTemp | vDspParam };

    static constexpr int userCreatableTypes = vDspALL;
}

class cDspVar // dsp variable
{
public:
    const QString& Name() const { return m_sName;}
    int size() const { return m_dspVarData.size(); }
    int valueTypeMask() const { return m_valueTypeMask; }
    DspDataType datatype() const { return m_dataType; }
    void setValue(int idx, float value);
    // Nightmare: This must go!!!
    float* data() { return m_dspVarData.data();}

    static int getInstanceCount();
private:
    friend class cDspMeasData;
    cDspVar(const QString &name, int size, int valueTypeMask,
            DspDataType dataType, DspSegmentType dspSegmentType);
    virtual ~cDspVar();
    void setData(QVector<float> data) {
        Q_ASSERT(m_dspVarData.size() == data.size());
        m_dspVarData = data;
    }

    QString m_sName; // a var. has its name
    const DspSegmentType m_dspSegmentType;
    int m_valueTypeMask; // an it can be of different type : vDspResult, vDspTemp , vDspIntVar , vDspParam
    DspDataType m_dataType; // it can be float or int
    QVector<float> m_dspVarData; // we hold an array for data storage
    static int m_instanceCount;
};
#endif // DSPVAR_H
