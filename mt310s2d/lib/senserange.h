#ifndef SENSERANGE_H
#define SENSERANGE_H

#include "scpiconnection.h"
#include "mt310s2justdata.h"
#include "adjustmentstatusinterface.h"
#include <scpi.h>

namespace SenseRange
{
enum Commands
{
    cmdType,
    cmdAlias,
    cmdAvail,
    cmdValue,
    cmdRejection,
    cmdOVRejection,
    cmdADWRejection
};
}

class cMTJustData;
class cSCPI;

class cSenseRange: public ScpiConnection, public AdjustmentStatusInterface
{
    Q_OBJECT

public:
    cSenseRange(cSCPI* scpiinterface,
                QString name,
                QString alias,
                bool avail,
                double rValue,
                double rejection,
                double ovrejection,
                double adcrejection,
                quint8 rselcode,
                quint16 mmask,
                JustRangeTripletOffsetGainPhase* justdata);
    ~cSenseRange();
    virtual void initSCPIConnection(QString leadingNodes) override;
    quint8 getAdjustmentStatus() override;

    QString& getName();
    double getUrvalue();
    quint8 getSelCode();
    quint16 getMMask();
    JustRangeTripletOffsetGainPhase* getJustData();
    bool isAvail();
    void setMMode(int m);

    void initJustData();
    void computeJustData();

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    QString m_ReadRangeType(QString& sInput);
    QString m_ReadRangeAlias(QString& sInput);
    QString m_ReadRangeAvail(QString& sInput);
    QString m_ReadRangeValue(QString& sInput);
    QString m_ReadRangeRejection(QString& sInput);
    QString m_ReadRangeOVRejection(QString& sInput);
    QString m_ReadRangeADWRejection(QString& sInput);

    QString m_sName; // the range name
    QString m_sAlias; // the range alias name
    bool m_bAvail; // range io avail or not
    double m_fRValue; // upper range value
    double m_fRejection; // 100% rejection value
    double m_fOVRejection; // overload rejection value
    double m_fADCRejection; // the adc's maximum rejection
    quint8 m_nSelCode; // selection code
    quint16 m_nMMask; // the possible measuring modes for this range
    quint8 m_nMMode; // the actual measuring mode
    JustRangeTripletOffsetGainPhase* m_pJustdata;

};


#endif // SENSERANGE_H
