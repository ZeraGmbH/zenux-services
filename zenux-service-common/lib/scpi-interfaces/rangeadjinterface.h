#ifndef RANGEADJINTERFACE_H
#define RANGEADJINTERFACE_H

#include "scpiconnection.h"
#include "permissionfunctions.h"
#include "abstractadjstatus.h"
#include "adjdataiteminterface.h"
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

static constexpr int GainCorrOrder = 3; // ax^3 + bx^2 + cx + d
static constexpr int PhaseCorrOrder  = 3;
static constexpr int OffsetCorrOrder = 3;

class RangeAdjInterface : public ScpiConnection
{
    Q_OBJECT
public:
    RangeAdjInterface(cSCPI* scpiinterface,
                             std::unique_ptr<AdjustScpiValueFormatter> adjustmentFormatter,
                             PermissionStructAdj permissions = PermissionStructAdj());
    virtual void initSCPIConnection(QString leadingNodes) override;
    AdjDataItemInterface* getAdjInterface(QString name);

    double getGainCorrectionSingle(double par);
    double getPhaseCorrectionSingle(double par);
    double getOffsetCorrectionSingle(double par);

    void Serialize(QDataStream&); // zum schreiben aller justagedaten in flashspeicher
    void Deserialize(QDataStream&); // zum lesen aller justagedaten aus flashspeicher
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
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
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

    AdjDataItemInterface m_gainCorrection;
    AdjDataItemInterface m_phaseCorrection;
    AdjDataItemInterface m_offsetCorrection;
    std::unique_ptr<AdjustScpiValueFormatter> m_scpiQueryFormatter;
    PermissionStructAdj m_permissions;
};

#endif
