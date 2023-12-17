#ifndef SENSERANGE_H
#define SENSERANGE_H

#include "scpiconnection.h"
#include "com5003justdata.h"
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
    cmdADCRejection
};
}

class Atmel;

class Com5003SenseRange : public ScpiConnection, public AdjustmentStatusInterface
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

    QString& getName();
    double getUrvalue();
    quint8 getSelCode();
    Com5003JustRangeTripletOffsetGainPhase* getJustData();
    bool getAvail();
    void setAvail(bool b);

    void initJustData();
    void computeJustData();

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    Atmel* m_pATMEL;
    QString m_sName; // the range name
    QString m_sAlias; // the range alias name
    bool m_bAvail; // range io avail or not
    double m_fRValue; // upper range value
    double m_fRejection; // 100% rejection value
    double m_fOVRejection; // overload rejection value
    double m_fADCRejection; // the adc's maximum rejection
    quint8 m_nSelCode; // selection code
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
