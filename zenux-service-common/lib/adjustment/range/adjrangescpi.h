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
    virtual void initSCPIConnection(QString leadingNodes) override;

    void setAdjGroupData(AdjDataRange groupData);
    AdjDataRange getAdjGroupData();

    AdjDataItemScpi* getAdjInterface(QString name);

    double getGainCorrectionSingle(double par);
    double getPhaseCorrectionSingle(double par);
    double getOffsetCorrectionSingle(double par);

    quint8 getAdjustmentStatus80Mask();
    // We ruined dc clamp 'on-the-fly DC offset adjustment' by da7ab853c5f1b70d888a5974e30fe63d923e90be
    // Not only that that 'on-the-fly DC offset adjustment' was sold to us as MANDATORY (but WinSAM
    // scripts were never implemented), it turned into a useful way to (manually) test power modules
    //
    // To fix it takes:
    // * TESTS on schnubbel protected ranges (internal/clamps) / on the fly allowed clamps
    // * Move permission query from scpiCmdInitJustData()/scpiCmdComputeJustData() into initJustData() / computeJustData()
    // * Make these methods return ZSCPI enum value
    // * Code corrections on init adj / compute caller loops to break on error and return enum
    // * Remove global permission query in SenseInterfaceCommon::scpiInitSenseAdjDataAllChannelRanges
    //   and SenseInterfaceCommon::scpiComputeSenseAdjDataAllChannelRanges
    // * finally make adjustment module handle errors so the caller gets aware...
    void initJustData();
    void computeJustData();

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
