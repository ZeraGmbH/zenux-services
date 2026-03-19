#ifndef DSPVARCLIENTINTERFACE_H
#define DSPVARCLIENTINTERFACE_H

#include <QString>
#include <QVector>

enum DspDataType { dspDataTypeInt, dspDataTypeFloat, dspDataTypeUnknown };

enum DspSegmentType { dspInternalSegment, moduleLocalSegment, moduleGlobalSegment, moduleAlignedMemorySegment };
static constexpr int minSegmentType = dspInternalSegment;
static constexpr int maxSegmentType = moduleAlignedMemorySegment;

class DspVarClientInterface
{
public:
    const QString& Name() const { return m_sName;}
    int size() const { return m_dspVarData.size(); }
    DspDataType datatype() const { return m_dataType; }
    DspSegmentType getSegmentType() const { return m_dspSegmentType; }
    void setValue(int idx, float value);
    // Nightmare: This must go!!!
    float* data() { return m_dspVarData.data();}

    static int getInstanceCount();
private:
    friend class DspVarGroupClientInterface;
    DspVarClientInterface(const QString &name, int size, DspDataType dataType, DspSegmentType dspSegmentType);
    virtual ~DspVarClientInterface();
    void setData(const QVector<float> &data);

    QString m_sName; // a var. has its name
    const DspSegmentType m_dspSegmentType;
    DspDataType m_dataType;
    QVector<float> m_dspVarData; // we hold an array for data storage
    static int m_instanceCount;
};
#endif // DSPVARCLIENTINTERFACE_H
