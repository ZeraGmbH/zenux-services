#ifndef SENSERANGE_H
#define SENSERANGE_H

#include "senserangecommon.h"
#include "mt310s2justdata.h"

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

class Mt310s2SenseRange: public SenseRangeCommon
{
    Q_OBJECT
public:
    Mt310s2SenseRange(cSCPI* scpiinterface,
                      QString name,
                      QString alias,
                      bool avail,
                      double rValue,
                      double rejection,
                      double ovrejection,
                      double adcrejection,
                      quint8 rselcode,
                      quint16 mmask,
                      Mt310s2JustRangeTripletOffsetGainPhase* justdata);
    ~Mt310s2SenseRange();
    virtual void initSCPIConnection(QString leadingNodes) override;
    quint8 getAdjustmentStatus() override;

    QString& getName();
    double getUrvalue();
    quint8 getSelCode();
    quint16 getMMask();
    Mt310s2JustRangeTripletOffsetGainPhase* getJustData();
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

    quint16 m_nMMask; // the possible measuring modes for this range
    quint8 m_nMMode; // the actual measuring mode
    Mt310s2JustRangeTripletOffsetGainPhase* m_pJustdata;

};


#endif // SENSERANGE_H
