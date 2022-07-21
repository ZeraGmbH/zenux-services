#ifndef SENSERANGE_H
#define SENSERANGE_H

#include <QObject>
#include <scpi.h>

#include <scpiconnection.h>
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

class cMTJustData;
class cSCPI;

class cSenseRange:public cSCPIConnection
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
                cMT310S2JustData* justdata);
    ~cSenseRange();
    virtual void initSCPIConnection(QString leadingNodes);
    quint8 getAdjustmentStatus();

    QString& getName();
    double getUrvalue();
    quint8 getSelCode();
    quint16 getMMask();
    cMT310S2JustData* getJustData();
    bool isAvail();
    void setMMode(int m);

    void initJustData();
    void computeJustData();

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

protected:
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

    QString m_ReadRangeType(QString& sInput);
    QString m_ReadRangeAlias(QString& sInput);
    QString m_ReadRangeAvail(QString& sInput);
    QString m_ReadRangeValue(QString& sInput);
    QString m_ReadRangeRejection(QString& sInput);
    QString m_ReadRangeOVRejection(QString& sInput);
    QString m_ReadRangeADWRejection(QString& sInput);

    cMT310S2JustData* m_pJustdata;

};


#endif // SENSERANGE_H
