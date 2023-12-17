#ifndef SENSERANGE_H
#define SENSERANGE_H

#include "senserangecommon.h"
#include "com5003justdata.h"


class Atmel;

class Com5003SenseRange : public SenseRangeCommon
{
    Q_OBJECT
public:
    Com5003SenseRange(cSCPI *scpiinterface,
                QString name,
                QString alias,
                bool avail,
                double rValue,
                double rejection,
                double ovrejection,
                double adcrejection,
                quint8 rselcode);
    ~Com5003SenseRange();
    virtual void initSCPIConnection(QString leadingNodes) override;
    quint8 getAdjustmentStatus() override;

    Com5003JustRangeTripletOffsetGainPhase* getJustData();

    void initJustData();
    void computeJustData();

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    Atmel* m_pATMEL;
    Com5003JustRangeTripletOffsetGainPhase* m_pJustdata;

    QString m_ReadRangeType(QString& sInput);
    QString m_ReadRangeAlias(QString& sInput);
    QString m_ReadRangeAvail(QString& sInput);
    QString m_ReadRangeValue(QString& sInput);
    QString m_ReadRangeRejection(QString& sInput);
    QString m_ReadRangeOVRejection(QString& sInput);
    QString m_ReadRangeADCRejection(QString& sInput);
};


#endif // SENSERANGE_H
