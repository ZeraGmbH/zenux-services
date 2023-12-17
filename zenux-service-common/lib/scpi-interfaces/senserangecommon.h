#ifndef SENSERANGECOMMON_H
#define SENSERANGECOMMON_H

#include "scpiconnection.h"
#include "adjustmentstatusinterface.h"

namespace SenseRange
{
enum Commands
{
    cmdType,
    cmdAlias,
    cmdAvail,
    cmdUpperRangeValue,
    cmdRejection,
    cmdOVRejection,
    cmdADCRejection
};
}

class SenseRangeCommon : public ScpiConnection, public AdjustmentStatusInterface
{
    Q_OBJECT
public:
    SenseRangeCommon(cSCPI* scpiInterface,
                     QString name,
                     QString alias,
                     bool avail,
                     double upperRangeValue, // = nominal
                     double rejection,
                     double ovrejection,
                     double adcrejection,
                     quint8 rselcode);
    void initSCPIConnection(QString leadingNodes) override;
    QString &getName(); // Ooohh - there are pointer kept
    double getUpperRangevalue() const;
    quint8 getSelCode() const;
    bool getAvail() const;
    void setAvail(bool avail); // It is just for Com5003 - do we need this really?
protected:
    // As long as sub-classes implement executeProtoScpi we cannot override
    bool execScpi(int cmdCode, cProtonetCommand* protoCmd);

    QString m_sName; // the range name
    const QString m_sAlias; // the range alias name
    bool m_bAvail; // range io avail or not
    const double m_upperRangeValue; // more a nominal value - we keep name for SCPI name
    const double m_fRejection; // 100% rejection value
    const double m_fOVRejection; // overload rejection value
    const double m_fADCRejection; // the adc's maximum rejection
    const quint8 m_nSelCode; // selection code
private:
    QString handeScpiRangeAlias(QString& sInput);
    QString handeScpiRangeAvail(QString& sInput);
    QString handeScpiRangeUpperRangeValue(QString& sInput);
};

#endif // SENSERANGECOMMON_H