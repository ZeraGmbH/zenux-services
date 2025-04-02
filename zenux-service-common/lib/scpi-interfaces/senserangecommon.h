#ifndef SENSERANGECOMMON_H
#define SENSERANGECOMMON_H

#include "scpiconnection.h"
#include "adjrangescpi.h"

class SenseRangeCommon : public ScpiConnection
{
    Q_OBJECT
public:
    static const char* rangeNameCurrentNull;
    static const char* rangeAliasNull;

    SenseRangeCommon(std::shared_ptr<cSCPI> scpiInterface,
                     const QString &name,
                     bool avail,
                     double upperRangeValue, // = nominal
                     double rejection,
                     double ovrejection,
                     quint8 rselcode,
                     quint32 typeFlags,
                     AdjRangeScpi* justdata,
                     int rejectionScpiQueryDigits);
    virtual ~SenseRangeCommon();
    void initSCPIConnection(QString leadingNodes) override;
    QString getRangeName();
    double getUpperRangevalue() const;
    quint8 getSelCode() const;
    bool getAvail() const;
    quint16 getMMask();
    void setMModeToUpdateAvailability(int mode);

    quint8 getAdjustmentStatus80Mask();
    AdjRangeScpi *getJustData();
    void initJustData();
    void computeJustData();

protected:
    bool m_bAvail; // range io avail or not
private:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    QString scpiRangeAlias(const QString& scpi) const;
    QString scpiRangeAvail(const QString& scpi) const;
    QString scpiRangeUpperRangeValue(const QString& scpi) const;
    QString scpiRangeRejection(const QString& scpi) const;
    QString scpiRangeOVRejection(const QString& scpi) const;
    QString scpiRangeADCRejection(const QString& scpi) const;
    QString scpiRangeTypeFlags(const QString& scpi) const;
    QString scpiControllerSelection(const QString& scpi) const;

    QString m_sName; // the range name
    const QString m_sAlias; // the range alias name
    const double m_upperRangeValue; // more a nominal value - we keep name for SCPI name
    const double m_fRejection; // 100% rejection value
    const double m_fOVRejection; // overload rejection value
    const quint8 m_nSelCode; // selection code
    const quint32 m_typeFlags;
    const int m_rejectionScpiQueryDigits;
    AdjRangeScpi* m_justdata;
};

#endif // SENSERANGECOMMON_H
