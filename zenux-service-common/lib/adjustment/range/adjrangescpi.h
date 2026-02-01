#ifndef ADJRANGESCPI_H
#define ADJRANGESCPI_H

#include "adjdatarange.h"
#include "scpiconnection.h"
#include "permissionfunctions.h"
#include "adjdataitemscpi.h"
#include <QDataStream>
#include <functional>
#include <memory>

// AdjustScpiValueFormatter(Factory) will go once we found a common format tested
// on all adjustment environments
struct AdjustScpiValueFormatter
{
    std::function<QString(double)> m_gainScpiFormatter;
    std::function<QString(double)> m_phaseScpiFormatter;
    std::function<QString(double)> m_offsetScpiFormatter;
    int m_correctionExportDigits;
};

class AdjustScpiValueFormatterFactory
{
public:
    static std::unique_ptr<AdjustScpiValueFormatter> createMt310s2AdjFormatter();
    static std::unique_ptr<AdjustScpiValueFormatter> createCom5003AdjFormatter();
};

class AdjRangeScpi : public ScpiConnection
{
    Q_OBJECT
public:
    AdjRangeScpi(std::shared_ptr<cSCPI> scpiinterface,
                 std::unique_ptr<AdjustScpiValueFormatter> adjustmentFormatter,
                 PermissionStructAdj permissions = PermissionStructAdj());
    virtual void initSCPIConnection(const QString &leadingNodes) override;

    void setAdjGroupData(AdjDataRange groupData);
    AdjDataRange getAdjGroupData();

    AdjDataItemScpi* getAdjInterface(QString name);

    double getGainCorrectionSingle(double par);
    double getPhaseCorrectionSingle(double par);
    double getOffsetCorrectionSingle(double par);

    quint8 getAdjustmentStatus80Mask();
    bool initJustData();
    bool computeJustData();

protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    virtual double getGainCorrectionTotal(double par);
    virtual double getPhaseCorrectionTotal(double par);
    virtual double getOffsetCorrectionTotal(double par);

private:
    QString scpiQueryGainCorrectionTotal(const QString &scpiInput);
    QString scpiQueryGainCorrectionSingle(QString &scpiInput);
    QString scpiQueryPhaseCorrectionTotal(QString &scpiInput);
    QString scpiQueryPhaseCorrectionSingle(QString &scpiInput);
    QString scpiQueryOffsetCorrectionTotal(QString &scpiInput);
    QString scpiQueryOffsetCorrectionSingle(QString &scpiInput);
    QString scpiQueryStatus(QString &scpiInput);
    QString scpiCmdComputeJustData(QString &scpiInput);
    QString scpiCmdInitJustData(QString &scpiInput); // done in Adjustmentmodule - left for compatibility

    AdjDataRange m_adjGroupData;
    AdjDataItemScpi m_gainCorrection;
    AdjDataItemScpi m_phaseCorrection;
    AdjDataItemScpi m_offsetCorrection;
    std::unique_ptr<AdjustScpiValueFormatter> m_scpiQueryFormatter;
    PermissionStructAdj m_permissions;
};

#endif
