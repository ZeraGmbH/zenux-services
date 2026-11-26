#ifndef SENSERANGECOMMON_H
#define SENSERANGECOMMON_H

#include "scpiconnection.h"
#include "adjrangescpi.h"

struct SenseRangeCommonConfig {
    const QString m_name;
    const double m_nominalValue; // aka urValue or rValue
    const double m_fRejection; // 100% rejection value
    const double m_fOVRejection; // overload rejection value
    const quint8 m_nSelCode; // selection code
    const quint32 m_typeFlags;
    const int m_rejectionScpiQueryDigits;
};

class SenseRangeCommon : public ScpiConnection
{
    Q_OBJECT
public:
    static const char* rangeNameCurrentNull;
    static const char* rangeAliasNull;

    SenseRangeCommon(const std::shared_ptr<cSCPI> &scpiInterface,
                     bool available,
                     const SenseRangeCommonConfig &config,
                     AdjRangeScpi* justdata);
    virtual ~SenseRangeCommon();
    void initSCPIConnection(const QString &leadingNodes);
    QString getRangeName();
    double getUpperRangevalue() const;
    quint8 getSelCode() const;
    bool getAvail() const;
    quint16 getMMask();
    void setMModeToUpdateAvailability(int mode);

    quint8 getAdjustmentStatus80Mask();
    AdjRangeScpi *getJustData();
    bool initJustData();
    bool computeJustData();

protected:
private:
    void executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd) override;
    QString scpiRangeAvail(const QString& scpi) const;
    QString scpiRangeUpperRangeValue(const QString& scpi) const;
    QString scpiRangeRejection(const QString& scpi) const;
    QString scpiRangeOVRejection(const QString& scpi) const;
    QString scpiRangeADCRejection(const QString& scpi) const;
    QString scpiRangeTypeFlags(const QString& scpi) const;
    QString scpiControllerSelection(const QString& scpi) const;

    bool m_available; // range io avail or not
    const SenseRangeCommonConfig m_config;
    AdjRangeScpi* m_justdata;
};

#endif // SENSERANGECOMMON_H
